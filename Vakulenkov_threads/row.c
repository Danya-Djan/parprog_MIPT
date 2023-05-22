#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem;
long double ans = 0;
int n;
int nproc;

void* slave_thr(void* my_rank) {
    long double psum = 0;
    int i = 0;
    for (i = *(int*)my_rank + 1; i <= n; i += nproc)
        psum += 1 / (long double)i;
    
    sem_wait(&sem);
    ans += psum;
    sem_post(&sem);
    printf("My rank is %d, my partial sum: %Lf\n", *(int*)my_rank, psum);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <num_threads> <n>\n", argv[0]);
        return 1;
    }

    nproc = atoi(argv[1]);
    n = atoi(argv[2]);
    if (nproc <= 0 || n <= 0) {
        printf("Number of threads and n must be positive integers.\n");
        return 1;
    }

    pthread_t* thr = (pthread_t*)malloc(nproc * sizeof(pthread_t));
    int* num = (int*)malloc(nproc * sizeof(int));
    if (thr == NULL || num == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }


    for (int i = 0; i < nproc; i++) {
        num[i] = i;
        if (pthread_create(&thr[i], NULL, slave_thr, (void*)(&num[i])) != 0) {
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

    printf("ANS: %Lf\n", ans);

    free(thr);
    free(num);

    return 0;
}
