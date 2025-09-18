// ./programming-with-shared-variables/iterative-parallelism/single-thread-app.cpp

#include <iostream>
#include <random>
#include <chrono>
#include <map>

using namespace std;
using namespace chrono;

mt19937 generator((random_device())());
uniform_int_distribution dist(-9, 9);

void fillRandom(int n, double* matrix);
void computeRow(int n, int i, const double* A, const double* B, double* C);
void multiplyMatrix(int n, int i, double* A, double* B, double* C);
void printMatrix(int n, const double* matrix);

void fillRandom(const int n, double* matrix) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i * n + j] = dist(generator);
        }
    }
}

void computeRow(const int n, const int i, const double* A, const double* B, double* C) {
    for (int j = 0; j < n; j++) {
        double sum = 0.0;

        for (int k = 0; k < n; k++) {
            sum += A[i * n + k] * B[k * n + j];
        }

        C[i * n + j] = sum;
    }
}

void multiplyMatrix(const int n, const double* A, const double* B, double* C) {
    for (int i = 0; i < n; i++) {
        computeRow(n, i, A, B, C);
    }
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
    map<int, long long> results;

    for (const int sizes[] = {10, 20, 50, 100, 200, 500, 1000}; int n : sizes) {
        auto* A = new double[n * n];
        auto* B = new double[n * n];
        auto* C = new double[n * n];

        fillRandom(n, A);
        fillRandom(n, B);

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                C[i * n + j] = 0.0;
            }
        }

        auto start = high_resolution_clock::now();
        multiplyMatrix(n, A, B, C);
        auto end = high_resolution_clock::now();

        const auto duration = duration_cast<milliseconds>(end - start).count();
        results[n] = duration;

        cout << n << " : " << duration << endl;

        printMatrix(n, A);
        delete[] A;

        printMatrix(n, B);
        delete[] B;

        printMatrix(n, C);
        delete[] C;

        cout << endl << string(120, *"-") << endl;
    }

    for (const auto& [fsr, snd] : results) {
        cout << fsr << " : " << snd << endl;
    }
}
