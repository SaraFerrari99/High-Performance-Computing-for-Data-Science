#ifndef UPDATE_PUZZLE_H
#define UPDATE_PUZZLE_H

#include "common.h"

bool update_puzzle_unsigned(char row_array[5][M], char col_array[5][6], char array_transposed[5][6], char puzzle[N][M], char last_array[N][M], char puzzle_reverse[N][M]);
bool check_if_array_completed(char puzzle_without_sign[5][M]);

#endif