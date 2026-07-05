#ifndef MPI_UPDATE_PUZZLE_H
#define MPI_UPDATE_PUZZLE_H

#include <mpi.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define M 64 // deve corrispondere al main
#define N 9

void transpose(char input[5][6], char output[5][6]);
bool update_puzzle_unsigned(char row_array[N][M], char col_array[5][6], char array_transposed[5][6], char puzzle[N][M],char last_array[N][M],char puzzle_reverse[N][M]);


#endif
