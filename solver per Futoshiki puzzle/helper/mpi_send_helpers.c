#include "mpi_send_helpers.h"

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
