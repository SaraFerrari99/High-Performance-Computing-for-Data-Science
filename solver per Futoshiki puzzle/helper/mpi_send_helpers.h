#ifndef MPI_SEND_HELPERS_H
#define MPI_SEND_HELPERS_H

#include <mpi.h>

#define M 64 // deve corrispondere al main

void send_row(char row[5][M]);
void send_column(char cols[5][6]);

#endif
