#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

const int TIME = 1;
const int SIZE = 1;
const double TAU = 0.001;
const double H = 0.001;

double f(double x, double t)
{
    return x + t;
}

double phi(double x) {
    return cos(x);
}

double psi(double t) {
    return exp(-t);
}

double** solve_eq() {
    double** u = (double**)malloc((int)(TIME / TAU) * sizeof(double*));
    for (int i = 0; i < TIME / TAU; i++) {
        u[i] = (double*)malloc((int)(SIZE / H) * sizeof(double));
        u[i][0] = psi(TAU * i);
    }
        
    for (int j = 0; j < SIZE / H; j++)
        u[0][j] = phi(H * j);

    
    for (int i = 1; i < TIME / TAU; i++) {
        for(int j = 1; j < SIZE / H; j++) {
            u[i][j] = (2 * f(H * (j + 1/2), TAU * (i + 1/2)) - 
                (1/TAU - 1/H) * u[i][j - 1] + (1/TAU + 1/H) * u[i - 1][j - 1]
                + (1/TAU - 1/H) * u[i - 1][j])/(1/TAU + 1/H);
        }
    }
    return u;
}


int main() {
    clock_t start_time = clock();
    double** u = solve_eq();
    clock_t stop_time = clock();
    
    FILE* fp = fopen("solution.txt", "w");
    
    for (int i = 0; i < TIME / TAU; i++) {
        for(int j = 0; j < SIZE / H; j++)
            fprintf(fp, "%lg ", u[i][j]);
        fprintf(fp, "\n");
    }
    
    fclose(fp);
    
    printf("Completed in %ld\n", stop_time - start_time);
    
    for (int i = 0; i < TIME / TAU; i++)
        free(u[i]);
    free(u);
    
    return 0;
}
