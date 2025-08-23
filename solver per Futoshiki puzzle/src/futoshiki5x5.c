#include <mpi.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "../helper/mpi_send_helpers.h"
#include "../helper/mpi_update_puzzle.h"
#include "../helper/mpi_check_void_helper.h"
#include "../helper/mpi_rules_helper.h"

#define N 9  // dimensione effettiva della griglia letta (numeri + simboli)
#define M 64 // caratteri per riga (N + '\0')

void set_puzzle(char puzzle[N][M], char puzzle_without_sign[5][M], char puzzle_without_sign_reverse[5][6], char puzzle_reverse[N][M])
{
    FILE *fp = fopen("../puzzle/puzzle1.txt", "r");
    if (!fp)
    {
        perror("Errore apertura file");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int i = 0; i < N; i++)
    {
        if (fgets(puzzle[i], M, fp) == NULL)
        {
            fprintf(stderr, "Errore lettura riga %d\n", i);
            fclose(fp);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        // Rimuove eventuale newline alla fine
        char *p = puzzle[i];
        while (*p)
        {
            if (*p == '\n')
            {
                *p = '\0';
                break;
            }
            p++;
        }
    }

    // Adesso riempiamo puzzle_without_sign SOLO con i numeri
    for (int i = 0; i < N; i += 2) // solo le righe "con numeri"
    {
        int k = 0;
        for (int j = 0; puzzle[i][j] != '\0'; j++)
        {
            if (isdigit((unsigned char)puzzle[i][j]))
            {
                puzzle_without_sign[i / 2][k++] = puzzle[i][j];
            }
        }
        puzzle_without_sign[i / 2][k] = '\0'; // termina la stringa
    }

    for (int c = 0; c < 5; c++)
    {
        for (int r = 0; r < 5; r++)
        {
            puzzle_without_sign_reverse[c][r] = puzzle_without_sign[r][c];
        }
        puzzle_without_sign_reverse[c][5] = '\0'; // terminatore di stringa
    }

    for (int c = 0; c < N; c++)
    {
        for (int r = 0; r < N; r++)
        {
            puzzle_reverse[c][r] = puzzle[r][c];
        }
        puzzle_reverse[c][5] = '\0'; // terminatore di stringa
    }

    fclose(fp);
}

int main(int argc, char *argv[])
{
    int rank, size;
    char puzzle[N][M];
    char puzzle_reverse[N][M];
    char puzzle_without_sign[5][M];
    char puzzle_without_sign_reverse[5][6];
    bool repeat = true;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        set_puzzle(puzzle, puzzle_without_sign, puzzle_without_sign_reverse,puzzle_reverse);

        while (repeat)
        {
            repeat = false;

            /*printf("\nPuzzle aggiornato (righe):\n");
            for (int i = 0; i < 5; i++) {
                printf("%s\n", puzzle_without_sign[i]);
            }

            printf("\nPuzzle aggiornato (colonne):\n");
            for (int i = 0; i < 5; i++) {
                printf("%s\n", puzzle_without_sign_reverse[i]);
            }*/

            // Manda righe e colonne ai worker
            send_row(puzzle_without_sign,puzzle);
            send_column(puzzle_without_sign_reverse,puzzle_reverse);

            // Riceve righe aggiornate
            for (int r = 1; r <= 5; r++)
            {

                char updated_row[6];
                bool local_repeat;
                MPI_Recv(updated_row, 6, MPI_CHAR, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&local_repeat, 1, MPI_C_BOOL, r, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                updated_row[5] = '\0';

                strcpy(puzzle_without_sign[r - 1], updated_row);

                // appena trovo un true, lo segno
                if (local_repeat)
                {
                    printf("trovato un true\n");
                    repeat = true;
                }
            }

            // Riceve colonne aggiornate
            for (int r = 6; r < 11; r++)
            {
                char updated_col[6];
                bool local_repeat;
                MPI_Recv(updated_col, 6, MPI_CHAR, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&local_repeat, 1, MPI_C_BOOL, r, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                strcpy(puzzle_without_sign_reverse[r - 6], updated_col);

                if (local_repeat)
                {
                    printf("trovato un true\n");
                    repeat = true;
                }
            }

            char array_transposed[5][6];
            update_puzzle_unsigned(puzzle_without_sign, puzzle_without_sign_reverse, array_transposed);
        }
        char stop_msg[6] = "STOP0"; // esattamente 6 caratteri
        for (int i = 1; i <= 10; i++)
        {
            MPI_Send(stop_msg, 6, MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
        printf("fuori dal while and END\n");
    }
    else if (rank >= 1 && rank <= 5)
    {
        while (1)
        {
            char my_row[6];
            char my_row_reverse[6];
            MPI_Recv(my_row, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(my_row_reverse, 6, MPI_CHAR, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (strncmp(my_row, "STOP", 4) == 0)
            {
                break;
            }

            printf("Processo %d ha ricevuto la riga: %s\n", rank, my_row);
            UpdateMessage res = apply_rules(my_row,my_row_reverse);
            printf("Processo %d nuova riga: %s\n", rank, my_row);

            MPI_Send(res.line, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&res.changed, 1, MPI_C_BOOL, 0, 1, MPI_COMM_WORLD);
        }
    }
    else if (rank >= 6 && rank <= 10)
    {
        while (1)
        {
            char my_col[6];
            char my_col_reverse[N];
            MPI_Recv(my_col, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(my_col_reverse, 6, MPI_CHAR, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (strncmp(my_col, "STOP", 4) == 0)
            {
                break;
            }

            printf("Processo %d ha ricevuto la colonna: %s\n", rank, my_col);
            UpdateMessage res = apply_rules(my_col,my_col_reverse);
            printf("Processo %d nuova colonna: %s\n", rank, my_col);

            MPI_Send(res.line, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&res.changed, 1, MPI_C_BOOL, 0, 1, MPI_COMM_WORLD);
            printf("send inviata");
        }
    }

    MPI_Finalize();
    return 0;
}
