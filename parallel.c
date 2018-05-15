#include <stdlib.h>
#include <omp.h>
#include "timing.c"
#include <string.h>


double **Matrix;
double **Matrix1;
int size;
double **Matrix_Mult_Result = NULL;

/* populating two matrices for multiplication */
void GetMatrix(char fileName[]) {

    int j;
    int i;
    char *token;
    size_t len = 0;
    ssize_t read;
    char *line = NULL;
    int lineCount = 0;
    char lineStr[15000];
    int numbersCount = 0;
    const char delim[2] = ",";
    FILE *fp = fopen("inputMatrix150_1.csv", "rb");

    // Check for file pointer
    if (fp == NULL) {
        printf("\nError reading file");
        exit(-1);
    }


    while ((read = getline(&line, &len, fp)) != -1) {
        sprintf(lineStr, "%s", line);

        numbersCount = 0;

        if (lineCount == 0) {

            // allocate the memory
            size = atoi(line);
            Matrix = (double **) malloc(size * sizeof(double *));
            Matrix1 = (double **) malloc(size * sizeof(double *));


            for (i = 0; i < size; i++) {
                Matrix[i] = (double *) malloc(size * sizeof(double));
                Matrix1[i] = (double *) malloc(size * sizeof(double));
            }
        } else if (lineCount > size) {
            break;
        } else {

            token = strtok(lineStr, delim);

            while (token != NULL) {
                sscanf(token, "%lf", &Matrix[lineCount - 1][numbersCount]);
                sscanf(token, "%lf", &Matrix1[lineCount - 1][numbersCount]);
                numbersCount++;
                token = strtok(NULL, delim);
            }

        }
    /*freeing memory*/
        if (line != NULL) {
            free(line);
            line = NULL;
        }

        lineCount++;
    }

    /*close file */
    fclose(fp);

}

double **Matrix;
double **Matrix1;
double **matrix_product = NULL;

int size;
int main(int argc, char *argv[]) {
    int i, j, k, num_threads, chunk;
    double result = 0;
    FILE *fp;
    char delim[2];
    num_threads = atoi(argv[1]);
    chunk = 20;


    GetMatrix("inputMatrix.csv");                 //function call to get matrix


    matrix_product = malloc(size * sizeof(double *));

    if (matrix_product == NULL) {
        printf("Memory has not been allocated");
        return 1;
    }
    timing_start();
/* parallel region start */
#pragma omp parallel shared (Matrix, result, matrix_product, num_threads, chunk) private(i, j, k)
    {
        omp_set_num_threads(num_threads);
#pragma omp for schedule(guided, chunk)
        for (i = 0; i < size; i++) {
            matrix_product[i] = (double *) malloc(size * sizeof(double));
        }


#pragma omp for reduction(+:result)
        for (i = 0; i < size; i++) {
            for (j = 0; j < size; j++) {
                result = 0;
                for (k = 0; k < size; k++) {
                    result = result + Matrix[i][k] * Matrix1[k][j];
                }

                matrix_product[i][j] = result;
            }
        }
    }
    timing_stop();
    print_timing();

/* for storing the output */
    fp = fopen("outputMatrix.csv", "w");
    fprintf(fp, "%d\n", size);

    for (j = 0; j < size; j++) {
        sprintf(delim, "%s", "");

        for (i = 0; i < size; i++) {
            fprintf(fp, "%s%.2f", delim, matrix_product[j][i]);
            sprintf(delim, "%s", ",");;
        }
        fprintf(fp, "\n");
    }

    fclose(fp);


    for (j = 0; j < size; j++) {
        free(matrix_product[j]);
        free(Matrix[j]);
    }
/* freeing the matrix */
    free(matrix_product);
    free(Matrix);

    return 0;
}
