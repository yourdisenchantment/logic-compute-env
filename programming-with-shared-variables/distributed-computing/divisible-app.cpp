// distributed-programming/mpi-divisible/divisible-app.cpp

#include <iostream>
#include <mpi.h>

using namespace std;

bool isDivisible(const int n) {
    return n % 11 == 0 || n % 13 == 0 || n % 17 == 0;
}

int countInRange(const int from, const int to) {
    int count = 0;
    for (int i = from; i <= to; i++) {
        if (isDivisible(i)) {
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int l = 0, r = 0;

    if (rank == 0) {
        cout << "Введите l и r: ";
        cin >> l >> r;
    }

    MPI_Bcast(&l, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // if (rank == 0) {
    //     for (int targetRank = 1; targetRank < size; targetRank++) {
    //         MPI_Send(&l, 1, MPI_INT, targetRank, 11, MPI_COMM_WORLD);
    //         MPI_Send(&r, 1, MPI_INT, targetRank, 12, MPI_COMM_WORLD);
    //     }
    // } else {
    //     MPI_Recv(&l, 1, MPI_INT, 0, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     MPI_Recv(&r, 1, MPI_INT, 0, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // }

    const int totalNumbers = r - l + 1;
    const int numbersPerProcess = totalNumbers / size;
    const int remainder = totalNumbers % size;

    const int myL = l + rank * numbersPerProcess + min(rank, remainder);
    int myR = myL + numbersPerProcess - 1;
    if (rank < remainder) {
        myR++;
    }

    const int localCount = countInRange(myL, myR);

    int globalCount = 0;
    MPI_Reduce(&localCount, &globalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Количество чисел: " << globalCount << endl;
    }

    MPI_Finalize();
    return 0;
}
