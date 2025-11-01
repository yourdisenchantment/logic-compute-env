// ./programming-with-shared-variables/producers-and-consumers/shared-variables.cpp

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

// Класс для безопасной передачи одного целого числа между потоками
class SharedInteger {
    int buf{}; // Буфер для хранения передаваемого числа
    bool hasData; // Флаг: true = есть данные, false = нет данных
    mutex mtx; // "Замок" для защиты от одновременного доступа
    condition_variable cv; // "Будильник" для пробуждения потоков

public:
    // Конструктор - изначально данных нет
    SharedInteger() : hasData(false) {}

    // Положить число в общую переменную
    void putValue(const int x) {
        unique_lock lock(mtx); // Закрываем "замок"

        // Ждем, пока другой поток заберет предыдущее число
        while (hasData) { // Пока есть данные - ждем
            cv.wait(lock); // Засыпаем и ждем сигнала
        }

        buf = x; // Кладем новое число
        hasData = true; // Помечаем: "данные есть"
        cv.notify_one(); // Будим другой поток
    }

    // Взять число из общей переменной
    int getValue() {
        unique_lock lock(mtx); // Закрываем "замок"

        // Ждем, пока другой поток положит число
        while (!hasData) { // Пока данных нет - ждем
            cv.wait(lock); // Засыпаем и ждем сигнала
        }

        const int y = buf; // Забираем число
        hasData = false; // Помечаем: "данных нет"
        cv.notify_one(); // Будим другой поток
        return y; // Возвращаем число
    }
};

// Функция для нахождения всех делителей числа
// Возвращает количество делителей, делители записывает в массив divisors
int findDivisors(const int n, int divisors[]) {
    if (n <= 0)
        return 0;

    int count = 0;

    // Ищем все делители от 1 до sqr(n)
    for (int i = 1; i * i <= n; i++) {
        if (n % i == 0) {
            divisors[count++] = i; // Добавляем i
            if (i != n / i) { // Если i != n/i
                divisors[count++] = n / i; // Добавляем парный делитель
            }
        }
    }

    // Быстрая сортировка вставками
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
    // Создаем "почтовый ящик" для обмена числами между потоками
    SharedInteger sharedData;

    // ПОТОК 1: Производитель (читает числа с клавиатуры)
    thread producer([&] { // [&] - можем использовать sharedData
        cout << "Введите последовательность целых чисел (завершение: -1):" << endl;

        while (true) {
            int num;
            cin >> num;
            sharedData.putValue(num); // Отправляем число второму потоку

            if (num == -1) {
                break;
            }
        }
    });

    // ПОТОК 2: Потребитель (находит и выводит делители)
    thread consumer([&] { // [&] - можем использовать sharedData
        int divisors[1000]; // Массив для хранения делителей (максимум 1000)

        while (true) {
            const int num = sharedData.getValue(); // Получаем число от первого потока

            if (num == -1) { // Если -1, то заканчиваем работу
                break;
            }

            // Находим все делители числа
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
