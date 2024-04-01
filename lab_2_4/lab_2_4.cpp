#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include "../lab_2_1/Initializer.cpp"

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    int message;
    MPI_Request send_request, recv_request;
    MPI_Status status;
    int x = check_process_count(argc, argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int dest = 0; dest < size; dest++) {
        if (dest != rank) {
            MPI_Isend(&rank, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &send_request);
            MPI_Irecv(&message, 1, MPI_INT, dest, 0, MPI_COMM_WORLD, &recv_request);

            MPI_Wait(&send_request, &status);
            MPI_Wait(&recv_request, &status);

            std::cout << "(" << rank << ") received message " << message << " from (" << dest << ")\n";
        }
    }

    MPI_Finalize();
    return 0;
}
