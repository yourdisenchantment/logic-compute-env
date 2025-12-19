// ./distributed-programming/mpi-divisible/divisible-app.cpp

#include <iostream>
#include <mpi.h>

using namespace std;

using ll = long long;

bool isDivisible(const ll n) {
    return n % 11 == 0 || n % 13 == 0 || n % 17 == 0;
}

ll countInRange(const ll from, const ll to) {
    ll count = 0;
    for (ll i = from; i <= to; i++) {
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

    ll l = 0, r = 0;

    if (rank == 0) {
        cout << "Введите l и r: ";
        cin >> l >> r;
    }

    MPI_Bcast(&l, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    const ll totalNumbers = r - l + 1;
    const ll numbersPerProcess = totalNumbers / size;
    const ll remainder = totalNumbers % size;

    const ll myL = l + rank * numbersPerProcess + min(static_cast<ll>(rank), remainder);
    ll myR = myL + numbersPerProcess - 1;
    if (rank < remainder) {
        myR++;
    }

    const ll localCount = countInRange(myL, myR);

    ll globalCount = 0;
    MPI_Reduce(&localCount, &globalCount, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Количество чисел: " << globalCount << endl;
    }

    MPI_Finalize();
    return 0;
}
