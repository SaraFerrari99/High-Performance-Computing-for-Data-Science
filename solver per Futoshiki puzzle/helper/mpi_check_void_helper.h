#ifndef MPI_CHECK_VOID_HELPER
#define MPI_CHECK_VOID_HELPER

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>

#define M 64 // deve corrispondere al main
#define N 9

bool check_only_one_void(char line[6]);

#endif