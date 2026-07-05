#include "mpi_update_puzzle.h"
#include <unistd.h>
#include "log.h"

void transpose(char input[5][6], char output[5][6])
{

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            output[j][i] = input[i][j];//TRANSPOSE ONLY NUMBERS, NOT THE SIGNS
        }
    }

    // add terminator
    for (int j = 0; j < 5; j++) {
        output[j][5] = '\0';
    }
}

bool different(char a[N][M], char b[N][M]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (a[i][j] != b[i][j]) {
                //FIX: Use 'k' instead of 'i' to avoid overwriting the outer index!
                for(int k = 0; k < 9; k++){
                    strcpy(b[k], a[k]);
                }
                return false;  
            }
        }
    }
    return true; 
}


bool update_puzzle_unsigned(char row_array[N][M], char col_array[5][6], char array_transposed[5][6],char puzzle[N][M], char last_array[N][M], char puzzle_reverse[N][M])
{
    //puzzle_without_sign, puzzle_without_sign_reverse, array_transposed,puzzle, puzzle_reverse
    //Transpose columns
    transpose(col_array, array_transposed);

    // Fusion: update row_array using array_transposed
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (row_array[i][j] == '0' && array_transposed[i][j] != '0') {
                row_array[i][j] = array_transposed[i][j];
            } else if (row_array[i][j] != '0' && array_transposed[i][j] == '0') {
                array_transposed[i][j] = row_array[i][j];
            }
        }
    }

    for (int c = 0; c < 5; c++)
    {
        for (int r = 0; r < 5; r++)
        {
            col_array[c][r] = array_transposed[r][c];
        }
        col_array[c][5] = '\0'; // terminate the string
    }

    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            puzzle[i*2][j*2] = row_array[i][j];
        }
    }

    //puzzle_reverse
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if(puzzle[i][j] == '^'){
                puzzle_reverse[j][i] = '<';
            }else{
                if(puzzle[i][j] == 'v'){
                    puzzle_reverse[j][i] = '>';
                }else{
                    puzzle_reverse[j][i] = puzzle[i][j];
                }
            }
        }
    }

    bool save_return = different(puzzle, last_array);

    return save_return;

}

