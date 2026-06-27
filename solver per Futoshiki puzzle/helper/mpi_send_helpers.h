#ifndef MPI_SEND_HELPERS_H
#define MPI_SEND_HELPERS_H

#include <mpi.h>
#include <stdio.h>

#define N 9  // dimensione effettiva della griglia letta (numeri + simboli)
#define M 64

void send_row(char row[5][M],char puzzle[N][M],char type[1]);
void send_column(char cols[5][6], char puzzle_reverse[N][M]);
void send_only_one_column(char cols[5], char puzzle_reverse[9]);


#endif
