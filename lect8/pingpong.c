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

    if (size < 2)
    {
        if (rank == 0)
        {
            printf("Questo programma richiede almeno 2 processi.\n");
        }
        MPI_Finalize();
        return 0;
    }
    while (1)
    {
        if (rank == 0)
        {
            if (count == 1)
            {
                MPI_Recv(&number, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                printf("Processo 0 ha ricevuto il numero %d dal processo 1.\n", number);
                sleep(2);
                MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("Processo 0 ha inviato il numero %d al processo 1.\n", number);
            }
            else
            {
                // Processo 0 invia il numero
                number = 42;
                MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
                printf("Processo 0 ha inviato il numero %d al processo 1.\n", number);
                count = 1;
                sleep(2);
            }
        }
        else if (rank == 1)
        {
            // Processo 1 riceve il numero
            MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Processo 1 ha ricevuto il numero %d dal processo 0.\n", number);
            sleep(2);
            MPI_Send(&number, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            printf("Processo 1 ha inviato il numero %d al processo 0.\n", number);
        }
    }
    MPI_Finalize(); // Termina MPI
    return 0;
}
