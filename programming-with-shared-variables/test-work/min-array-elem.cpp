// ./programming-with-shared-variables/test-work/min-array-elem.cpp

#include <iostream>
#include <random>
#include <thread>

using namespace std;

mt19937 generator((random_device())());
uniform_int_distribution value_dist(-100, 100);
uniform_int_distribution size_dist(10, 20);

void findMinAndFill(int arr[], int start, int end);

void findMinAndFill(int* arr, int start, int end) {
    const int size = end - start;

    if (size <= 1) {
        return;
    }

    const int mid = start + (size / 2);

    thread leftThread(findMinAndFill, arr, start, mid);
    thread rightThread(findMinAndFill, arr, mid, end);

    leftThread.join();
    rightThread.join();

    const int leftMin = arr[start];
    const int rightMin = arr[mid];

    const int overallMin = (leftMin < rightMin) ? leftMin : rightMin;

    for (int i = start; i < end; ++i) {
        arr[i] = overallMin;
    }
}

int main() {
    const int size = size_dist(generator);
    const auto arr = new int[size];

    for (int i = 0; i < size; ++i) {
        arr[i] = value_dist(generator);
    }

    cout << "Исходный массив (размер " << size << "): ";
    for (int i = 0; i < size; ++i)
        cout << arr[i] << " ";
    cout << endl;

    findMinAndFill(arr, 0, size);

    cout << "Массив после обработки: ";
    for (int i = 0; i < size; ++i)
        cout << arr[i] << " ";
    cout << endl;

    cout << "Найденный минимум: " << arr[0] << endl;

    delete[] arr;
}
