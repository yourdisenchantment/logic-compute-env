// programming-with-shared-variables/distributed-computing/multiply-app.cpp

#include <iostream>
#include <random>
#include <mpi.h>

using namespace std;

mt19937 generator((random_device())());
uniform_int_distribution dist(-9, 9);

constexpr int N = 10;

double A[N][N];
double B[N][N];
double C[N][N];

void fillRandom(const int n, double matrix[N][N]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = dist(generator);
        }
    }
}

void printMatrix(const int n, const double matrix[N][N], const string &name) {
    cout << name << ":" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
    cout << endl;
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

int main(int argc, char **argv) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        fillRandom(N, A);
        fillRandom(N, B);
    }

    MPI_Bcast(&A[0][0], N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B[0][0], N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = rank; i < N; i += size) {
        computeRow(N, i);
    }

    for (int i = 0; i < N; i++) {
        const int owner = i % size;
        MPI_Bcast(C[i], N, MPI_DOUBLE, owner, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        cout << "Processes: " << size << endl << endl;

        printMatrix(N, A, "A");
        printMatrix(N, B, "B");
        printMatrix(N, C, "C");
    }

    MPI_Finalize();
    return 0;
}
