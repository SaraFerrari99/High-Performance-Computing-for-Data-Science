#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int data;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Il root (rank 0) inizializza il dato
    if (rank == 0) {
        data = 42; // dato da trasmettere
        printf("Processo %d inizializza data = %d\n", rank, data);
    } else {
        // Ogni processo diverso da 0 riceve dal proprio genitore nell'albero
        int parent = (rank - 1) / 2;
        MPI_Recv(&data, 1, MPI_INT, parent, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Processo %d ha ricevuto data = %d da processo %d\n", rank, data, parent);
    }

    // Calcolo figli nell'albero binario (se esistono)
    int left_child = 2 * rank + 1;
    int right_child = 2 * rank + 2;

    // Invio ai figli
    if (left_child < size) {
        MPI_Send(&data, 1, MPI_INT, left_child, 0, MPI_COMM_WORLD);
        printf("Processo %d ha inviato data = %d a processo %d\n", rank, data, left_child);
    }
    if (right_child < size) {
        MPI_Send(&data, 1, MPI_INT, right_child, 0, MPI_COMM_WORLD);
        printf("Processo %d ha inviato data = %d a processo %d\n", rank, data, right_child);
    }

    MPI_Finalize();
    return 0;
}
