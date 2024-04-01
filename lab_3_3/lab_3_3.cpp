#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <iostream>
#include <vector>
#include "../lab_2_1/Initializer.cpp"

void print_results(const char* prompt, const std::vector<std::vector<int>>& matrix) {
    std::cout << "\n\n" << prompt << "\n";
    for (const auto& row : matrix) {
        for (int value : row) {
            std::cout << " " << value;
        }
        std::cout << std::endl;
    }
    std::cout << "\n\n";
}

void fillRandom(std::vector<std::vector<int>>& matrix, int n) {
    std::srand(std::time(nullptr));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = std::rand() % 10;
        }
    }
}

std::vector<int> flatten(const std::vector<std::vector<int>>& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    std::vector<int> flattened(rows * cols);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            flattened[i * cols + j] = matrix[i][j];
        }
    }

    return flattened;
}

std::vector<std::vector<int>> unflatten(const std::vector<int>& flattened, int rows, int cols) {
    std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = flattened[i * cols + j];
        }
    }

    return matrix;
}

void matrix_vector_multiply(const std::vector<int>& aa, const std::vector<std::vector<int>>& b, std::vector<int>& cc, int N, int size) {
    for (int i = 0; i < N / size; i++) {
        for (int j = 0; j < N; j++) {
            int sum = 0;
            for (int k = 0; k < N; k++) {
                sum += aa[i * N + k] * b[k][j];
            }
            cc[i * N + j] = sum;
        }
    }
}


int main(int argc, char* argv[]) {
    check_process_count(argc, argv);
    double start_time, end_time;

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int N;
    if (rank == 0) {
        std::cout << "Enter the value of N: ";
        std::cin >> N;
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (N % size != 0) {
        if (rank == 0) {
            std::cerr << "Error: N is not divisible by the number of processes!" << std::endl;
        }
        MPI_Finalize();
        exit(1);
    }

    int num_elements_per_proc = N * N / size;
    std::vector<std::vector<int>> a(N, std::vector<int>(N));
    std::vector<std::vector<int>> b(N, std::vector<int>(N, 0));
    std::vector<std::vector<int>> c(N, std::vector<int>(N, 0));
    std::vector<int> flattened_a(N * N);
    std::vector<int> flattened_b(N * N);
    std::vector<int> flattened_c(N * N, 0);
    std::vector<int> aa(num_elements_per_proc);
    std::vector<int> cc(num_elements_per_proc, 0);

    if (rank == 0) {
        fillRandom(a, N);
        fillRandom(b, N);
        print_results("A = ", a);
        print_results("B = ", b);
        flattened_a = flatten(a);
        flattened_b = flatten(b);
    }

    start_time = MPI_Wtime();
    MPI_Scatter(flattened_a.data(), num_elements_per_proc, MPI_INT, aa.data(), num_elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(flattened_b.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0) {
        b = unflatten(flattened_b, N, N);
    }

    matrix_vector_multiply(aa, b, cc, N, size);

    MPI_Gather(cc.data(), N * N / size, MPI_INT, flattened_c.data(), N * N / size, MPI_INT, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    if (rank == 0) {
        std::cout << "MPI_Scatter/Gather time: " << (end_time - start_time) * 10e3 << " microseconds" << std::endl;
    }

    start_time = MPI_Wtime();
    if (rank == 0) {
        for (int dest = 1; dest < size; ++dest) {
            MPI_Send(flattened_a.data() + dest * num_elements_per_proc, num_elements_per_proc, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(aa.data(), num_elements_per_proc, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Bcast(flattened_b.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        b = unflatten(flattened_b, N, N);
    }

    matrix_vector_multiply(aa, b, cc, N, size);

    if (rank == 0) {
        for (int src = 1; src < size; ++src) {
            MPI_Recv(flattened_c.data() + src * num_elements_per_proc, num_elements_per_proc, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else {
        MPI_Send(cc.data(), N * N / size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    end_time = MPI_Wtime();

    if (rank == 0) {
        std::cout << "MPI_Send/Recv time: " << (end_time - start_time) * 10e3 << " microseconds" << std::endl;
    }

    if (rank == 0) {
        c = unflatten(flattened_c, N, N);
        print_results("C = ", c);
    }

    MPI_Finalize();

    return 0;
}
