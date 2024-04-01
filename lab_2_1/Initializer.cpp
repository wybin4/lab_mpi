#include <iostream>

int check_process_count(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Enter the number of processes\n";
        exit(1);
    }

    int x = std::atoi(argv[1]);

    if (x < 2) {
        std::cerr << "The number of processes is too small\n";
        exit(1);
    }
    return x;
}