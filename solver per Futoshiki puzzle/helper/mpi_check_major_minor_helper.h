#ifndef MPI_CHECK_MAJOR_MINOR_HELPER
#define MPI_CHECK_MAJOR_MINOR_HELPER

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define M 64 // deve corrispondere al main

bool check_minor_major(char line_reverse[10], char line[6]);

#endif