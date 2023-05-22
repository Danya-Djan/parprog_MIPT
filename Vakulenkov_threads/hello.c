#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int nproc;

void* slave_thr(void* my_rank) {
    printf("Hello World, my rank is %d, total number of executors is %d\n", *(int*)my_rank, nproc);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    nproc = atoi(argv[1]);
    if (nproc <= 0) {
        printf("Number of threads must be a positive integer.\n");
        return 1;
    }

    pthread_t* thr = (pthread_t*)malloc(nproc * sizeof(pthread_t));
    int* num = (int*)malloc(nproc * sizeof(int));
    if (thr == NULL || num == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("nproc: %d\n", nproc);

    for (int i = 0; i < nproc; i++) {
        num[i] = i;
        if (pthread_create(&thr[i], NULL, slave_thr, (void*)(num + i)) != 0) {
            printf("Failed to create thread %d.\n", i);
            return 1;
        }
    }

    for (int i = 0; i < nproc; ++i) {
        if (pthread_join(thr[i], NULL) != 0) {
            printf("Failed to join thread %d.\n", i);
            return 1;
        }
    }

    free(thr);
    free(num);

    return 0;
}
