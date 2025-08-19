#ifndef MPI_UPDATE_PUZZLE_H
#define MPI_UPDATE_PUZZLE_H

#include <mpi.h>
#include <stdbool.h>

#define M 64 // deve corrispondere al main
#define N 9

bool check_only_one_void(char line[6]);

#endif