#include <mpi.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define N 9  // dimensione effettiva della griglia letta (numeri + simboli)
#define M 64 // caratteri per riga (N + '\0')

void set_puzzle(char puzzle[N][M], char puzzle_without_sign[5][M], char puzzle_without_sign_reverse[5][6])
{
    FILE *fp = fopen("puzzle1.txt", "r");
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

    fclose(fp);
}

void send_row(char row[5][M])
{
    for (int i = 0; i < 5; i++)
    {
        MPI_Send(row[i], 6, MPI_CHAR, i + 1, 0, MPI_COMM_WORLD);
    }
}

void send_column(char cols[5][6])
{
    for (int c = 0; c < 5; c++)
    {
        MPI_Send(cols[c], 6, MPI_CHAR, c + 6, 0, MPI_COMM_WORLD);
    }
}

void check_only_one_void(char check[1][5]){
    
}

int main(int argc, char *argv[])
{
    int rank, size;
    char puzzle[N][M];
    char puzzle_without_sign[5][M];
    char puzzle_without_sign_reverse[5][6];
    bool repeat = false;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        set_puzzle(puzzle, puzzle_without_sign, puzzle_without_sign_reverse);

        printf("Processo %d ha letto il puzzle:\n", rank);
        for (int i = 0; i < N; i++)
        {
            printf("%s\n", puzzle[i]);
        }

        printf("Processo %d ha letto il puzzle:\n", rank);
        for (int i = 0; i < 5; i++)
        {
            printf("%s\n", puzzle_without_sign[i]);
        }

        send_row(puzzle_without_sign);
        send_column(puzzle_without_sign_reverse);
    }
    else if (rank >= 1 && rank <= 5)
    {
        char my_row[6]; // 5 cifre + terminatore
        MPI_Recv(my_row, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d ha ricevuto la riga: %s\n", rank, my_row);
        check_only_one_void(my_row);
    }
    else if (rank >= 6 && rank <= 10)
    {
        char my_col[6];
        MPI_Recv(my_col, 6, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d ha ricevuto la colonna: %s\n", rank, my_col);
        check_only_one_void(my_col);
    }

    MPI_Finalize();
    return 0;
}
