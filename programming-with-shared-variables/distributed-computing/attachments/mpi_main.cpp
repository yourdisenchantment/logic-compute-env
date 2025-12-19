#include <iostream>
#include <mpi.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) * size + rank);

    int steps;
    if (rank == 0) {
        cin >> steps;
    }

    MPI_Bcast(&steps, 1, MPI_INT, 0, MPI_COMM_WORLD);
    cout << steps << endl;

    int inside = 0;
    for (int iter = 0; iter < steps; iter++) {
        double x = rand() / double(RAND_MAX);
        double y = rand() / double(RAND_MAX);
        double r = (x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5);
        if (r < 0.5 * 0.5)
            inside++;
    }

    // int values[size];

    // MPI_Gather( &inside, 1, MPI_INT, values, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int sum;
    MPI_Reduce(&inside, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        /*int sum = 0;
        for(int i=0;i<size;i++)
            sum+= values[i];
        cout << sum / (double(steps) * size) *4 << endl;*/
        cout << sum / (double(steps) * size) * 4 << endl;
    }

    MPI_Finalize();
}
