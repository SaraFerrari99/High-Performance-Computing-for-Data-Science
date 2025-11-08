#ifndef MPI_CHECK_CROSS
#define MPI_CHECK_CROSS

#include <mpi.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "mpi_check_void_helper.h"
#include "mpi_check_major_minor_helper.h"
#include "mpi_number_between.h"
#include <unistd.h>

#define P 10

typedef struct {
    char col[6];     // indica se la regola ha fatto modifiche
    char row[6];     // qui metti la riga o colonna (5 + terminatore '\0')
} UpdateLine;

UpdateLine cross_rules(char column[P], char row[P], int rank, int number_of_column);

#endif