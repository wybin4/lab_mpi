#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int new_rank, new_size;
    int message = 0;
    MPI_Comm even_comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Comm_split(MPI_COMM_WORLD, (rank % 2 == 0), rank, &even_comm);
    if (rank % 2 == 0) {
        message = rank;
        MPI_Bcast(&message, 1, MPI_INT, 0, even_comm);

        MPI_Comm_rank(even_comm, &new_rank);
        MPI_Comm_size(even_comm, &new_size);
        printf("proc %d (%d) received message = %d\n", new_rank, rank, message);
    }

    MPI_Finalize();
    return 0;
}
