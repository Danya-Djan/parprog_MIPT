#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAG 0

int main(int argc, char** argv) {
    int rank, size;
    int message_size = 1000;
    char* message_send = (char*)malloc(message_size * sizeof(char));
    char* message_recv = (char*)malloc(message_size * sizeof(char));
    double start_time, end_time;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        printf("This program requires exactly 2 processes.\n");
        MPI_Finalize();
        exit(1);
    }

    for (int i = 0; i < message_size; i++) {
        message_send[i] = (char)i;
    }

    if (rank == 0) {
        start_time = MPI_Wtime();
        MPI_Send(message_send, message_size, MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
        MPI_Recv(message_recv, message_size, MPI_CHAR, 1, TAG, MPI_COMM_WORLD, &status);
        end_time = MPI_Wtime();
    } else {
        MPI_Recv(message_recv, message_size, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
        MPI_Send(message_send, message_size, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        printf("Message transmission delay: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    free(message_send);
    free(message_recv);
    return 0;
}

// mpicc dopusk.c -o dopusk
// mpirun -np 2 dopusk