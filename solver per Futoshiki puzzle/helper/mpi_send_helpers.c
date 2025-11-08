#include "mpi_send_helpers.h"

void send_row(char row[5][M], char puzzle[N][M],char type[1])
{
    for (int i = 0; i < 5; i++)
    {
        if(type[0] == '1'){
            MPI_Send(row[i], 6, MPI_CHAR, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(puzzle[i * 2], 9, MPI_CHAR, i + 1, 2, MPI_COMM_WORLD);
            MPI_Send(&puzzle[0][0],45,MPI_CHAR,i+1,6,MPI_COMM_WORLD);
        }else{
            if(type[0] == '2'){
                MPI_Send(puzzle[i*2], 9, MPI_CHAR, i + 11, 7, MPI_COMM_WORLD);
                printf("MANDATO DA SEND DA 0 A %i\n",i+11);
            }
        }
    }
}

void send_column(char cols[5][6], char puzzle_reverse[N][M])
{
    for (int c = 0; c < 5; c++)
    {
        MPI_Send(cols[c], 6, MPI_CHAR, c + 6, 0, MPI_COMM_WORLD);
        MPI_Send(puzzle_reverse[c * 2], 9, MPI_CHAR, c + 6, 2, MPI_COMM_WORLD);
    }
}

void send_only_one_column(char cols[5], char puzzle_reverse[9]){
    for(int i = 0; i < 5; i++){
        MPI_Send(puzzle_reverse, 9, MPI_CHAR, i + 11, 8, MPI_COMM_WORLD);
    }
}
