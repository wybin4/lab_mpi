#include <iostream>
#include <cmath>
#include <mpi.h>

#define N 100000000

double calculate_pi(int rank, int size, int n) {
    double sum = 0.0;
    for (int i = rank; i < n; i += size) {
        double xi = (i + 0.5) / n;
        sum += 4.0 / (1.0 + xi * xi);
    }
    return sum;
}

int main(int argc, char* argv[]) {
    int my_rank;
    int num_procs;
    double pi_local = -1;
    double pi_global;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    start_time = MPI_Wtime();
    pi_local = calculate_pi(my_rank, num_procs, N);
    MPI_Reduce(&pi_local, &pi_global, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    if (my_rank == 0) {
        std::cout << "MPI_Reduce time: " << end_time - start_time << " seconds" << std::endl;
    }

    pi_local = -1;

    start_time = MPI_Wtime();
    if (my_rank != 0) {
        pi_local = calculate_pi(my_rank, num_procs, N);
        MPI_Send(&pi_local, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else {
        double pi_buffer;
        for (int src = 1; src < num_procs; ++src) {
            MPI_Recv(&pi_buffer, 1, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            pi_local += pi_buffer;
        }
        pi_local += calculate_pi(my_rank, num_procs, N);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    if (my_rank == 0) {
        std::cout << "MPI_Send/MPI_Recv time: " << end_time - start_time << " seconds" << std::endl;
    }

    if (my_rank == 0) {
        double pi = pi_global / N;
        std::cout.precision(15);
        std::cout << "pi = " << std::fixed << pi << std::endl;
    }

    MPI_Finalize();
    return 0;
}
