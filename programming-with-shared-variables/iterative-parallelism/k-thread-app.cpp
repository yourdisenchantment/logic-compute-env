// ./programming-with-shared-variables/iterative-parallelism/k-thread-app.cpp

#include <chrono>
#include <iostream>
#include <random>
#include <thread>

using namespace std;
using namespace chrono;

mt19937 generator((random_device())());
uniform_int_distribution dist(-9, 9);

constexpr int N = 1000;

double A[N][N];
double B[N][N];
double C[N][N];

constexpr int K = 50;
thread threads[K];

void fillRandom(int n, double matrix[N][N]);
void computeRows(int n, int startRow, int endRow);
void multiplyMatrix(int n, int numThreads);

void fillRandom(const int n, double matrix[N][N]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = dist(generator);
        }
    }
}

void computeRows(const int n, const int startRow, const int endRow) {
    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;

            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }

            C[i][j] = sum;
        }
    }
}

void multiplyMatrix(const int n, const int numThreads) {
    const int rowsPerThread = n / numThreads;
    const int remainder = n % numThreads;

    int startRow = 0;

    for (int t = 0; t < numThreads; t++) {
        int endRow = startRow + rowsPerThread + (t < remainder ? 1 : 0);
        threads[t] = thread(computeRows, n, startRow, endRow);
        startRow = endRow;
    }

    for (int t = 0; t < numThreads; t++) {
        if (threads[t].joinable()) {
            threads[t].join();
        }
    }
}


int main() {
    fillRandom(N, A);
    fillRandom(N, B);

    for (int k = 1; k <= 50; k++) {
        for (auto& i : C) {
            for (double& j : i) {
                j = 0.0;
            }
        }

        auto start = high_resolution_clock::now();
        multiplyMatrix(N, k);
        auto end = high_resolution_clock::now();

        duration<double, milli> d = end - start;

        cout << k << d.count() << endl;
    }
}
