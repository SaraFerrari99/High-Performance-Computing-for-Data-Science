#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

/*int main(int argc, char *argv[])
{
    int rank, size;
    int number;
    int local_value;
    int global_sum;

    MPI_Init(&argc, &argv);               // Inizializza MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Ottiene il rank del processo
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Ottiene il numero totale di processi

    int values[] = {0, 1, 4, 4};

    if (size != 4)
    {
        if (rank == 0)
        {
            printf("Questo programma richiede almeno 4 processi.\n");
        }
        MPI_Finalize();
        return 0;
    }

    local_value = values[rank];

    printf("Processo %d ha valore %d\n", rank, local_value);

    MPI_Reduce(&local_value, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("La somma totale è di: %d", global_sum);
    }

    MPI_Finalize(); // Termina MPI
    return 0;
}*/

int main(int argc, char *argv[])
{
    int rank, size;
    int number;

    MPI_Init(&argc, &argv);               // Inizializza MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Ottiene il rank del processo
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Ottiene il numero totale di processi

    int values[] = {1, 4, 4};

    if (size != 4)
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
        number = 9;
    }
    MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);

    printf("il processo %d ha ricevuto il dato %d\n", rank, number);

    MPI_Finalize(); // Termina MPI
    return 0;
}
