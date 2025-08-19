#ifndef MPI_UPDATE_PUZZLE_H
#define MPI_UPDATE_PUZZLE_H

#include <mpi.h>

#define M 64 // deve corrispondere al main
#define N 9

void transpose(char input[5][6], char output[5][6]);
void update_puzzle_unsigned(char row_array[N][M], char col_array[5][6], char array_transposed[5][6]);

#endif
