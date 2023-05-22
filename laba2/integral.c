#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define DYNAMIC_STEP

pthread_mutex_t mutex;
int num_threads = 0;
int num_tasks = 0;
int n_task = 0;

typedef struct ThreadArgs {
    double (*f)(double);
    double start;
    double stop;
    double accuracy;
} ThreadArgs;

double linear(double x) {
    return x;
}

double quad(double x) {
    return x * x;
}

double my_func(double x) {
    return sin(1 / (x - 5));
}

#ifdef DYNAMIC_STEP

double integral(double (*f)(double), double start_n, double stop_n, double accuracy) {
    double integr1 = 0.5 * (f(start_n) + f(stop_n)) * (stop_n - start_n);
    double middle = (start_n + stop_n) / 2;
    double integr2 = 0.5 * (f(start_n) + f(middle)) * (middle - start_n) +
                   0.5 * (f(middle) + f(stop_n)) * (stop_n - middle);

    if (fabs(integr1 - integr2) < accuracy)
        return integr2;

    return integral(f, start_n, middle, accuracy / 2) + integral(f, middle, stop_n, accuracy / 2);
}

#else

double integral(double (*f)(double), double start_n, double stop_n, double step) {
    double integr = 0.0;
    double x = start_n;
    while (x < stop_n && stop_n - x >= step) {
        integr += 0.5 * (f(x + step) + f(x)) * step;
        x += step;
    }

    return integr;
}

#endif

void* thread_func(void* arg) {
    ThreadArgs* thread_args = (ThreadArgs*)arg;
    double start = thread_args->start;
    double stop = thread_args->stop;
    double accuracy = thread_args->accuracy;
    double (*f)(double) = thread_args->f;

    double* integr = (double *)malloc(sizeof(double));
    *integr = 0.0;

    double interval = (stop - start) / (double)num_tasks;

    while (true) {
        pthread_mutex_lock(&mutex);
        int cur_task = n_task++;
        pthread_mutex_unlock(&mutex);

        if (cur_task >= num_tasks)
            break;

        double stop_n = interval * (cur_task + 1) + start;
        double start_n = interval * cur_task + start;

        *integr += integral(f, start_n, stop_n, accuracy);
    }

    return integr;
}

double integrate(double (*f)(double), double start, double stop, double accuracy) {
    double integral = 0.0;

    void* part_int;
    pthread_t threads[num_threads];

    ThreadArgs thread_args = { f, start, stop, accuracy };
    n_task = 0;

    for (int i = 0; i < num_threads; i++)
        pthread_create(&threads[i], NULL, thread_func, &thread_args);

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], &part_int);
        integral += *(double*)part_int;
        free(part_int);
    }

    return integral;
}

int main(int argc, char** argv) {
    if (argc < 4)
        return 1;

    num_threads = atoi(argv[1]);
    num_tasks = pow(num_threads, 4);
    double start = atof(argv[2]);
    double stop = atof(argv[3]);
    double accuracy = 0.0001;

    if (argc > 4)
        accuracy = atof(argv[4]);

    pthread_mutex_init(&mutex, NULL);

    double result = integrate(my_func, start, stop, accuracy);

    printf("Result: %lf\n", result);

    pthread_mutex_destroy(&mutex);

    return 0;

}


// gcc -pthread -o integral integral.c
// ./integral 4 0.0 1.0 0.0001


