#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int number;
    int iterations;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2 || argc < 2)
    {
        if (rank == 0)
        {
            printf("Questo programma richiede almeno 2 processi e/o il numero di iterazioni volute.\n");
        }
        MPI_Finalize();
        return 0;
    }

    if (argc > 1)
    {
        iterations = atoll(argv[1]);
    }

    number = 42;

    double start = MPI_Wtime();

    for (long long i = 0; i < iterations; i++)
    {
        if (rank == 0)
        {
            MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
            printf("[Iterazione %d] Processo 0 ha inviato %d\n", i, number);
            MPI_Recv(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("[Iterazione %d] Processo 0 ha ricevuto %d\n", i, number);
        }
        else if (rank == 1)
        {
            MPI_Recv(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("[Iterazione %d] Processo 1 ha ricevuto %d\n", i, number);
            MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            printf("[Iterazione %d] Processo 1 ha inviato %d\n", i, number);
        }
    }

    double end = MPI_Wtime();
    double total = end - start;

    if (rank == 0)
    {

        printf("Il tempo utilizzato per concludere le operazioni è stato di %f secondi", total);
    }

    MPI_Finalize();
    return 0;
}
