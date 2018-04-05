#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "timing.c"

int main(int argc, char *argv[]) {
    int i, count, samples, nthreads, seed, chunk;
    struct drand48_data drand_buf;                		        // structure for random
    double x, y;

    samples = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    omp_set_num_threads(nthreads);
    chunk = atoi(argv[3]);

    count = 0;
    timing_start();
#pragma omp parallel private(i, x, y, seed, drand_buf) shared(samples)   // omp started with private and shared variables
    {
        seed = 1235486518 + omp_get_thread_num() * 2999;                 // seed value initialized
        srand48_r(seed, &drand_buf);

#pragma omp for reduction(+:count)
        for (i = 0; i < samples; i++) {
            drand48_r(&drand_buf, &x);                        		// gets x cordinate
            drand48_r(&drand_buf, &y);                        		// gets y cordinate
            if (x * x + y * y <= 1.0) count++;                		// formula
        }
    }
    timing_stop();
    printf("Estimate of pi: %7.5f\n", 4.0 * count / samples);
    print_timing();
}


