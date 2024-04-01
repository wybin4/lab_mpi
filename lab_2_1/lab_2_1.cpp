#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include "Initializer.cpp"

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    int message = 0;
    int x = check_process_count(argc, argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        MPI_Send(&message, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
        MPI_Recv(&message, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "(" << rank << ") received message " << message << "\n";
    }
    else {
        MPI_Recv(&message, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "(" << rank << ") received message " << message << "\n";
        message++;
        MPI_Send(&message, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
