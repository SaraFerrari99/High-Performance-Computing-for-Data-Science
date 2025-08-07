#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int number;
    int count = 0;

    MPI_Init(&argc, &argv);               // Inizializza MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Ottiene il rank del processo
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Ottiene il numero totale di processi

    int next = (rank + 1) % size;
    int prev = (rank - 1 + size) % size;
    int lastprocess = size - 1;

    if (size < 4)
    {
        if (rank == 0)
        {
            printf("Questo programma richiede almeno 4 processi.\n");
        }
        MPI_Finalize();
        return 0;
    }

    if (rank == 0)
    {
        // Processo 0 invia il numero
        number = 42;
        MPI_Send(&number, 1, MPI_INT, 1, rank, MPI_COMM_WORLD);
        printf("Processo 0 ha inviato il numero %d al processo 1 con id %d.\n", number, rank);
        MPI_Recv(&number, 1, MPI_INT, prev, prev, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo 0 ha ricevuto il numero %d dal processo %d con id %d.\n", number, prev, rank);
        sleep(2);
    }
    else
    {
        if (rank == lastprocess)
        {
            MPI_Recv(&number, 1, MPI_INT, prev, prev, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Processo %d ha ricevuto il numero %d dal processo %d con id %d.\n", rank, number, prev, prev);
            return 0;
        }
        else
        {
            MPI_Recv(&number, 1, MPI_INT, prev, prev, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Processo %d ha ricevuto il numero %d dal processo %d con id %d.\n", rank, number, prev, prev);
            sleep(2);
            MPI_Send(&number, 1, MPI_INT, next, rank, MPI_COMM_WORLD);
            printf("Processo %d ha inviato il numero %d al processo %d con id %d.\n", rank, number, next, rank);
        }
    }

    MPI_Finalize(); // Termina MPI
    return 0;
}
