// ./programming-with-shared-variables/iterative-parallelism/single-thread-app.cpp

#include <iostream>
#include <random>
#include <chrono>

using namespace std;
using namespace chrono;

mt19937 generator((random_device())());
uniform_int_distribution dist(-9, 9);

constexpr int N = 1000;
double A[N][N];
double B[N][N];
double C[N][N];

void fillRandom(int n, double matrix[N][N]);
void computeRow(int n, int i);
void multiplyMatrix(int n);

void fillRandom(const int n, double matrix[N][N]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = dist(generator);
        }
    }
}

void computeRow(const int n, const int i) {
    for (int j = 0; j < n; j++) {
        double sum = 0.0;

        for (int k = 0; k < n; k++) {
            sum += A[i][k] * B[k][j];
        }

        C[i][j] = sum;
    }
}

void multiplyMatrix(const int n) {
    for (int i = 0; i < n; i++) {
        computeRow(n, i);
    }
}

int main() {
    fillRandom(N, A);
    fillRandom(N, B);

    cout << "| n | time |" << endl;
    cout << "|---|------|" << endl;

    for (const int n : {10, 20, 50, 100, 200, 500, 1000}) {
        for (auto& i : C) {
            for (double& j : i) {
                j = 0.0;
            }
        }

        auto start = high_resolution_clock::now();
        multiplyMatrix(n);
        auto end = high_resolution_clock::now();

        duration<double, milli> d = end - start;
        cout << "| " << n << " | " << d.count() << " |" << endl;
    }
}
