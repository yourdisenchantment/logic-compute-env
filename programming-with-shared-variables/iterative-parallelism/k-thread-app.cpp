// ./programming-with-shared-variables/iterative-parallelism/k-thread-app.cpp

#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <map>

using namespace std;
using namespace chrono;

mt19937 generator((random_device())());
uniform_int_distribution dist(-9, 9);

void fillRandom(int n, double* matrix);
void computeRowsRange(int n, int startRow, int endRow, const double* A, const double* B, double* C);
void multiplyMatrix(int n, int numThreads, const double* A, const double* B, double* C);
void printMatrix(int n, const double* matrix);

void fillRandom(const int n, double* matrix) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i * n + j] = dist(generator);
        }
    }
}

void computeRowsRange(const int n, const int startRow, const int endRow, const double* A, const double* B, double* C) {
    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;

            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }

            C[i * n + j] = sum;
        }
    }
}

void multiplyMatrix(int n, int numThreads, const double* A, const double* B, double* C) {
    if (numThreads <= 0)
        numThreads = 1;

    if (numThreads > n)
        numThreads = n;

    auto* threads = new thread[numThreads];

    const int rowsPerThread = n / numThreads;
    const int remainder = n % numThreads;

    int startRow = 0;

    for (int t = 0; t < numThreads; t++) {
        int endRow = startRow + rowsPerThread + (t < remainder ? 1 : 0);
        threads[t] = thread(computeRowsRange, n, startRow, endRow, A, B, C);
        startRow = endRow;
    }

    for (int t = 0; t < numThreads; t++) {
        if (threads[t].joinable()) {
            threads[t].join();
        }
    }

    delete[] threads;
}

void printMatrix(int n, const double* matrix) {
    n = min(10, n);
    cout << endl;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i * n + j] << " ";
        }

        cout << endl;
    }
}

int main() {
    constexpr int n = 1000;
    map<int, long long> results;

    auto* A = new double[n * n];
    auto* B = new double[n * n];

    fillRandom(n, A);
    fillRandom(n, B);

    for (int k = 1; k <= 50; k++) {
        auto* C = new double[n * n];

        for (int i = 0; i < n * n; i++) {
            C[i] = 0.0;
        }

        auto start = high_resolution_clock::now();
        multiplyMatrix(n, k, A, B, C);
        auto end = high_resolution_clock::now();

        const auto duration = duration_cast<milliseconds>(end - start).count();
        results[k] = duration;

        cout << k << " : " << duration << endl;

        delete[] C;
    }

    delete[] A;
    delete[] B;
}
