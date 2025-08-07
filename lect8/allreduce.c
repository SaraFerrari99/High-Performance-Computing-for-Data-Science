#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int global_sum = 0;
    int shared_sum = 0;

    MPI_Init(&argc, &argv);               
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    int values[] = {1, 4, 4};  // Valori da inviare
    int local_value = 0;

    if (size != 4) {
        if (rank == 0)
            printf("Questo programma richiede esattamente 4 processi.\n");
        MPI_Finalize();
        return 0;
    }

    if (rank != 0) {
        local_value = values[rank - 1];
        MPI_Send(&local_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("Processo %d ha inviato %d a 0\n", rank, local_value);
    }

    if (rank == 0) {
        int ricevuto;
        for (int i = 1; i < size; i++) {
            MPI_Recv(&ricevuto, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Processo 0 ha ricevuto %d dal processo %d\n", ricevuto, i);
            global_sum += ricevuto;
        }
    }

    // Tutti i processi ricevono la somma da 0 tramite Allreduce
    MPI_Allreduce(&global_sum, &shared_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    printf("Processo %d vede la somma totale: %d\n", rank, shared_sum);

    MPI_Finalize();
    return 0;
}
