#include "mpi_update_puzzle.h"

void transpose(char input[5][6], char output[5][6])
{
    printf("input in transponse\n");
    for(int i = 0; i < 5; i++){
       printf("%s\n", input[i]);
    }

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            output[j][i] = input[i][j];  // trasponi solo i numeri
        }
    }

    // aggiungi terminatore
    for (int j = 0; j < 5; j++) {
        output[j][5] = '\0';
    }
}

void update_puzzle_unsigned(char row_array[N][M], char col_array[5][6], char array_transposed[5][6])
{
    // Trasponi le colonne
    transpose(col_array, array_transposed);

    printf("array trasposto\n");
    for(int i = 0; i < 5; i++){
        printf("%s\n", array_transposed[i]);
    }

    printf("array row\n");
    for(int i = 0; i < 5; i++){
        printf("%s\n", row_array[i]);
    }

    // Fusione: aggiorna row_array usando array_transposed
    printf("new array fused:\n");
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (row_array[i][j] == '0' && array_transposed[i][j] != '0') {
                row_array[i][j] = array_transposed[i][j];
            } else if (row_array[i][j] != '0' && array_transposed[i][j] == '0') {
                array_transposed[i][j] = row_array[i][j];
            }
            // Se entrambi diversi da zero o entrambi zero, rimangono invariati
            printf("%c", row_array[i][j]);
        }
        printf("\n");
    }

    for (int c = 0; c < 5; c++)
    {
        for (int r = 0; r < 5; r++)
        {
            col_array[c][r] = array_transposed[r][c];
        }
        col_array[c][5] = '\0'; // terminatore di stringa
    }
}