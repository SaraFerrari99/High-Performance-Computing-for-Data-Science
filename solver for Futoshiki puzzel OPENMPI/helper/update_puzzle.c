#include "update_puzzle.h"
#include <string.h>

bool update_puzzle_unsigned(char row_array[5][M], char col_array[5][6], char array_transposed[5][6], char puzzle[5][M], char last_array[5][M], char puzzle_reverse[5][M]) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) array_transposed[j][i] = col_array[i][j];
        array_transposed[i][5] = '\0';
    }
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (row_array[i][j] == '0' && array_transposed[i][j] != '0') row_array[i][j] = array_transposed[i][j];
            else if (row_array[i][j] != '0' && array_transposed[i][j] == '0') array_transposed[i][j] = row_array[i][j];
        }
    }
    for (int c = 0; c < 5; c++) {
        for (int r = 0; r < 5; r++) col_array[c][r] = array_transposed[r][c];
        col_array[c][5] = '\0';
    }
    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++) puzzle[i*2][j*2] = row_array[i][j];

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if(puzzle[i][j] == '^') puzzle_reverse[j][i] = '<';
            else if(puzzle[i][j] == 'v') puzzle_reverse[j][i] = '>';
            else puzzle_reverse[j][i] = puzzle[i][j];
        }
    }
    
    bool different = false;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (puzzle[i][j] != last_array[i][j]) different = true;
        }
    }
    if (different) {
        for(int k = 0; k < 9; k++) strcpy(last_array[k], puzzle[k]);
        return false;
    }
    return true;
}

bool check_if_array_completed(char puzzle_without_sign[5][M]) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (puzzle_without_sign[i][j] == '0') return true; 
        }
    }
    return false;
}