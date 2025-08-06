 #include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Inizializza l'ambiente MPI
    MPI_Init(NULL, NULL);

    // Ottieni il numero totale di processi
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Ottieni il rank (ID) del processo corrente
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Stampa il messaggio
    printf("Hello from process rank %d out of %d processors\n", world_rank, world_size);

    // Finalizza l'ambiente MPI
    MPI_Finalize();
}
