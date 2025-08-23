#ifndef MPI_CHECK_MAJOR_MINOR_HELPER
#define MPI_CHECK_MAJOR_MINOR_HELPER

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>

#define M 64 // deve corrispondere al main
#define N 9

bool check_minor_major(char line_reverse[6], char line[6]);

#endif