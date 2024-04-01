#include <iostream>
#include <string>
#include <cstring>
#include <mpi.h>

#define MAX_STRING_LENGTH 1000

void get_character_count(int my_rank, int p, char* global_str, int n, int* local_count) {
    int chunk_size = n / p;
    int remainder = n % p;
    int local_chunk_size = (my_rank < remainder) ? chunk_size + 1 : chunk_size;
    int start = my_rank * chunk_size + ((my_rank < remainder) ? my_rank : remainder);
    int end = start + local_chunk_size;
    for (int i = start; i < end; i++) {
        local_count[(int)global_str[i]]++;
    }
}

int main(int argc, char* argv[]) {
    int my_rank;
    int p;
    char local_str[MAX_STRING_LENGTH] = "";
    int local_count[256] = { 0 };
    char global_str[MAX_STRING_LENGTH] = "";
    int global_count[256] = { 0 };
    int n = -1;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (my_rank == 0) {
        std::cout << "Enter the length of the string (n): ";
        std::cin >> n;
        std::cout << "Enter a string:" << std::endl;
        std::cin.ignore();
        std::cin.getline(global_str, MAX_STRING_LENGTH);
        global_str[n] = '\0';
    }

    start_time = MPI_Wtime();
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(global_str, sizeof(global_str), MPI_CHAR, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    if (my_rank == 0) {
        std::cout << "MPI_Bcast time: " << (end_time - start_time) * 10e6 << " microseconds" << std::endl;
    }

    get_character_count(my_rank, p, global_str, n, local_count);
    MPI_Reduce(local_count, global_count, 256, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    memset(local_count, 0, sizeof(local_count));
    memset(global_count, 0, sizeof(global_count));

    start_time = MPI_Wtime();
    if (my_rank == 0) {
        for (int dest = 1; dest < p; dest++) {
            MPI_Send(global_str, sizeof(global_str), MPI_CHAR, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&n, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(global_str, sizeof(global_str), MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    if (my_rank == 0) {
        std::cout << "Point-to-point communication time: " << (end_time - start_time) * 10e6 << " microseconds" << std::endl;
    }

    get_character_count(my_rank, p, global_str, n, local_count);
    MPI_Reduce(local_count, global_count, 256, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        if (my_rank == 0) {
            std::cout << "Character counts:" << std::endl;
            for (int i = 0; i < 256; i++) {
                if (global_count[i] > 0) {
                    std::cout << (char)i << ": " << global_count[i] << std::endl;
                }
            }
        }
    }

    MPI_Finalize();
    return 0;
}
