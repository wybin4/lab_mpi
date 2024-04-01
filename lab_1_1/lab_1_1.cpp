#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            std::cerr << "Usage: " << argv[0] << " <number of processes>\n";
        }
        MPI_Finalize();
        return 1;
    }

    int x = std::stoi(argv[1]);

    if (x > size) {
        if (rank == 0) {
            std::cerr << "Error: Number of processes specified (" << x << ") exceeds available MPI processes (" << size << ")!\n";
        }
        MPI_Finalize();
        return 1;
    }

    if (rank < x) {
        std::cout << "I am " << rank << " process from " << size << " processes!\n";
    }

    MPI_Finalize();
    return 0;
}
