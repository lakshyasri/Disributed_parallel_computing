#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "timing.c"

int main(int argc, char *argv[]) {
    int i, count, samples, seed;
    struct drand48_data drand_buf;                            // structure for random
    double x, y;

    samples = atoi(argv[1]);


    count = 0;
    timing_start();
    {
        seed = 1202107158;                    		      //seed value inintialized
        srand48_r(seed, &drand_buf);

        for (i = 0; i < samples; i++) {
            drand48_r(&drand_buf, &x);                        // gets x cordinate
            drand48_r(&drand_buf, &y);			      // gets y cordinate
            if (x * x + y * y <= 1.0) count++;                // formula	
        }
    }
    timing_stop();
    printf("Estimate value of pi is: %7.5f\n", 4.0 * count / samples);
    print_timing();
}


