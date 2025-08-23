#include "mpi_send_helpers.h"

void send_row(char row[5][M], char puzzle[N][M])
{
    for (int i = 0; i < 5; i++)
    {
        MPI_Send(row[i], 6, MPI_CHAR, i + 1, 0, MPI_COMM_WORLD);
        MPI_Send(puzzle[i*2], 6, MPI_CHAR, i + 1, 2, MPI_COMM_WORLD);
    }

}

void send_column(char cols[5][6],char puzzle_reverse[N][M])
{
    for (int c = 0; c < 5; c++)
    {
        MPI_Send(cols[c], 6, MPI_CHAR, c + 6, 0, MPI_COMM_WORLD);
        MPI_Send(cols[c*2], 6, MPI_CHAR, c + 6, 2, MPI_COMM_WORLD);
    }
}
