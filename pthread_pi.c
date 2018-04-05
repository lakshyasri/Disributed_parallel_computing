#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "timing.c"

long c = 0;
long partial_thread;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *runt() {
    long count = 0;

    struct drand48_data drand_buf;            		  // Structure for random
    double x, y, seed;
    long i;
    seed = 1202107158;                    		  //seed value inintialized
    srand48_r(seed, &drand_buf);

    for (i = 0; i < partial_thread; i++) {

        drand48_r(&drand_buf, &x);                        // gets x cordinate
        drand48_r(&drand_buf, &y);			  // gets y cordinate
        if (x * x + y * y < 1) {
            count++;
        }
    }

    pthread_mutex_lock(&mutex);
    c += count;
    pthread_mutex_unlock(&mutex);
}


int main(int argc, const char *argv[]) {
    int samples, thread;
    samples = atoi(argv[1]);
    thread = atoi(argv[2]);
    partial_thread = samples / thread;
    srand((unsigned) time(NULL));

    pthread_t *threads = malloc(thread * sizeof(pthread_t));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    timing_start();
    int i;
    for (i = 0; i < thread; i++) {
        pthread_create(&threads[i], &attr, runt, (void *) NULL);
    }

    for (i = 0; i < thread; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(threads);

    printf("Pi: %f\n", (4.0 * (double) c) / ((double) partial_thread * thread));
    timing_stop();
    print_timing();

    return 0;
}

