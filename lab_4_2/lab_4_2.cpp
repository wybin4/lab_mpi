#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void error(const char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

int main(int argc, char* argv[]) {
    int world_size, universe_size, * universe_sizep, flag;
    MPI_Comm everyone;           /* intercommunicator */
    char worker_program[100];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 1)
        error("Top heavy with management");

    MPI_Comm_get_attr(MPI_COMM_WORLD, MPI_UNIVERSE_SIZE, &universe_sizep, &flag);
    if (!flag) {
        printf("This MPI does not support UNIVERSE_SIZE. How many\nprocesses total?");
        if (scanf_s("%d", &universe_size) != 1)
            error("Invalid input");
    }
    else
        universe_size = *universe_sizep;

    if (universe_size == 1)
        error("No room to start workers");

    sprintf_s(worker_program, sizeof(worker_program), "./worker_process");

    MPI_Comm_spawn(worker_program, MPI_ARGV_NULL, universe_size - 1, MPI_INFO_NULL, 0, MPI_COMM_SELF, &everyone, 0);

    int parent_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &parent_rank);

    int spawned_rank, spawned_size;
    MPI_Comm_rank(everyone, &spawned_rank);
    MPI_Comm_size(everyone, &spawned_size);

    printf("I am %d process from %d processes! My parent is %d\n", spawned_rank, spawned_size, parent_rank);

    MPI_Finalize();
    return 0;
}
