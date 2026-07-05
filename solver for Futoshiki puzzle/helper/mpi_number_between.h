#ifndef MPI_CHECK_NUMBER_BETWEEN_HELPER
#define MPI_CHECK_NUMBER_BETWEEN_HELPER

#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define M 64 // deve corrispondere al main

bool check_number_between(char line_reverse[10], char line[6]);

#endif