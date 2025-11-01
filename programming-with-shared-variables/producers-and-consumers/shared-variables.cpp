// ./programming-with-shared-variables/producers-and-consumers/shared-variables.cpp

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

class SharedInteger {
    int buf{};
    bool hasData;
    mutex mtx;
    condition_variable cv;

public:
    SharedInteger() : hasData(false) {}

    void putValue(const int x) {
        unique_lock lock(mtx);

        while (hasData) {
            cv.wait(lock);
        }

        buf = x;
        hasData = true;
        cv.notify_one();
    }

    int getValue() {
        unique_lock lock(mtx);

        while (!hasData) {
            cv.wait(lock);
        }

        const int y = buf;
        hasData = false;
        cv.notify_one();
        return y;
    }
};

int findDivisors(const int n, int divisors[]) {
    if (n <= 0)
        return 0;

    int count = 0;

    for (int i = 1; i * i <= n; i++) {
        if (n % i == 0) {
            divisors[count++] = i;

            if (i != n / i) {
                divisors[count++] = n / i;
            }
        }
    }

    for (int i = 1; i < count; i++) {
        const int key = divisors[i];
        int j = i - 1;

        while (j >= 0 && divisors[j] > key) {
            divisors[j + 1] = divisors[j];
            j--;
        }
        divisors[j + 1] = key;
    }

    return count;
}

int main() {
    SharedInteger sharedData;

    thread producer([&] { // [&] - можем использовать sharedData
        cout << "Введите последовательность целых чисел (завершение: -1):" << endl;

        while (true) {
            int num;
            cin >> num;
            sharedData.putValue(num);

            if (num == -1) {
                break;
            }
        }
    });

    thread consumer([&] {
        int divisors[1000];

        while (true) {
            const int num = sharedData.getValue();

            if (num == -1) {
                break;
            }

            const int count = findDivisors(num, divisors);

            cout << "Делители числа " << num << ": ";
            for (int i = 0; i < count; i++) {
                cout << divisors[i];

                if (i < count - 1) {
                    cout << " ";
                }
            }
            cout << endl;
        }
    });

    producer.join();
    consumer.join();
}
