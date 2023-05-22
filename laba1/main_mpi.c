#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#define TAG_PREV 5
#define TAG_SOLVED 6

const int TIME = 1;
const int SIZE = 1;
const double TAU = 0.001;
const double H = 0.001;

double f(double x, double t) {
    return x + t;
}

double phi(double x) {
    return cos(x);
}

double psi(double t) {
    return exp(-t);
}

int main(int argc, char** argv) {
    int size = 0;
    int rank = 0;

    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    clock_t start_time = clock();

    int start_num = rank * (int)(SIZE / H) / size;
    int stop_num = (rank + 1) * (int)(SIZE / H) / size - 1;

    int cur_size = stop_num - start_num + 1;
    if (rank != 0)
        cur_size += 1;

    double** u_cur = (double**)calloc((int)(TIME / TAU), sizeof(double *));

    for (int i = 0; i < (int)(TIME / TAU); i++) {
        u_cur[i] = (double *)calloc(cur_size, sizeof(double));
        if (i == 0) {
            for (int j = 0; j < cur_size; j++)
                u_cur[0][j] = (rank == 0) ? phi(H * (j + start_num)) : phi(H * (j + start_num - 1));
        }

        else {
            if (rank == 0)
                u_cur[i][0] = psi(i * TAU);

            else
                MPI_Recv(&(u_cur[i][0]), 1, MPI_DOUBLE, rank - 1, TAG_PREV, MPI_COMM_WORLD, &status);

            for (int j = 1; j < cur_size; j++) {
                u_cur[i][j] = (2 * f(H * (j + 1/2), TAU * (i + 1/2)) -
                    (1/TAU - 1/H) * u_cur[i][j - 1] + (1/TAU + 1/H) * u_cur[i - 1][j - 1]
                    + (1/TAU - 1/H) * u_cur[i - 1][j])/(1/TAU + 1/H);
            }

            if (rank != size - 1)
                MPI_Send(&(u_cur[i][cur_size - 1]), 1, MPI_DOUBLE, rank + 1, TAG_PREV, MPI_COMM_WORLD);
        }
    }


    if (rank == 0)
    {
        double** u = (double**)calloc((int)(TIME / TAU), sizeof(double *));

        for (int i = 0; i < (int)(TIME / TAU); i++) {
            u[i] = (double*) calloc ((int)(SIZE / H), sizeof(double));
            memcpy(u[i], u_cur[i], cur_size * sizeof(double));

        }

        for (int r = 1; r < size; r++) {
            int start_num_r = r * (int)(SIZE / H) / size;
            int stop_num_r = (r + 1) * (int)(SIZE / H) / size - 1;
            int cur_size_r = stop_num_r - start_num_r + 2;

            for (int i = 0; i < (int)(TIME / TAU); i++)
                MPI_Recv(u[i] + start_num_r, cur_size_r - 1, MPI_DOUBLE, r, TAG_SOLVED, MPI_COMM_WORLD, &status);
        }

        clock_t stop_time = clock();
        printf("Completed in %ld\n", stop_time - start_time);

        FILE* fp;

        fp = fopen("solution_mpi.txt", rank == 0 ? "w" : "a");

        for (int j = 0; j < (int)(SIZE / H); j++) {
          for (int i = 0; i < (int)(TIME / TAU); i++)
            fprintf(fp, "%lg ", u[i][j]);

          fprintf(fp, "\n");
        }

        fclose(fp);
    }

    else {
        for (int i = 0; i < (int)(TIME / TAU); i++)
            MPI_Send(u_cur[i] + 1, cur_size - 1, MPI_DOUBLE, 0, TAG_SOLVED, MPI_COMM_WORLD);
    }



    MPI_Finalize();

    return 0;
}
