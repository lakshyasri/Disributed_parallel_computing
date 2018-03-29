// csv file not uploaded the the generetor too, use any double set csv value with input file name for successful compilation @lakshyasri@outlook.com

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/time.h>
#include<time.h>
#include<math.h>
#include<string.h>

struct timeval tm1, tm2;
long long t;

void timing_start() {
    gettimeofday(&tm1, NULL);
}

void timing_stop() {
    gettimeofday(&tm2, NULL);

    t = (tm2.tv_sec - tm1.tv_sec) * 1000000 + (tm2.tv_usec - tm1.tv_usec);
}

void print_timing() {
    printf("%lld microseconds occured\n", t);
}

long long get_timing() {
    return t;
}


// Size of Vector and Matrix.
int row1, col1, col2, currentRow = 0;
double **InMat1, **InMat2, **ResMat;

pthread_t *threads;

int numberOfThreads;

/* Mutex for the currentRow. */
pthread_mutex_t mutex_Row = PTHREAD_MUTEX_INITIALIZER;


void *operator(int Id) {

    int i, j, myRow;

    while (1) {

        //Locking

        pthread_mutex_lock(&mutex_Row);

        if (currentRow >= row1) {

            pthread_mutex_unlock(&mutex_Row);

            if (Id == 0)

                return 0;

            pthread_exit(0);

        }
        myRow = currentRow;

        currentRow++;

        // Thread working

        pthread_mutex_unlock(&mutex_Row);

        // unlocking Row

        for (j = 0; j < col2; j++)

            for (i = 0; i < col1; i++)

                ResMat[myRow][j] += InMat1[myRow][i] * InMat2[i][j];


    }
}


int main(int argc, char *argv[]) {

    clock_t start, end;
    double cpu_time_used, N;
    FILE *fp;
    srand(time(0));
    time_t t;
    FILE *fp1;

    /*
       Hold for Threads.  pthread_t *threads;  int numberOfThreads;
         */

    int i, j;

    if (argc < 1) {

        printf("\n Insufficient argumets. \n Usage:");
        printf(" exe row1 col1 col2 threads.\n");
        return 0;
    }

    numberOfThreads = abs(atoi(argv[1]));
    N = abs(atoi(argv[2]));

    row1 = N;
    col1 = N;
    col2 = N;

    printf("\nNumber of threads: %d", numberOfThreads);

    InMat1 = malloc(N * sizeof(double));
    InMat2 = malloc(N * sizeof(double));
    ResMat = malloc(N * sizeof(double));

    for (i = 0; i < N; i++)
        InMat1[i] = malloc(sizeof(double) * N);
    for (i = 0; i < N; i++)
        InMat2[i] = malloc(sizeof(double) * N);
    for (i = 0; i < N; i++)
        ResMat[i] = malloc(sizeof(double) * N);



    /* Populate the Matrices from csv File */
    int size = N;

    char buffer[1024];
    char *record, *line;
    char *a;
    int mat[100][100];
    int temp[100];
    FILE *fstream = fopen("inputMatrix.csv", "r");
    if (fstream == NULL) {
        printf("\n file opening failed ");
        return -1;
    }
    while ((line = fgets(buffer, sizeof(buffer), fstream)) != NULL) {
        record = strtok(line, ",");
        while (record != NULL) {
            //copy all elements of the matrix into 1d array
            temp[i++] = atoi(record);
            record = strtok(NULL, ",");
        }
        //prepare the matrix

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                InMat1[i][j] = temp[i * size + j];
            }
        }


        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                InMat2[i][j] = temp[i * size + j];
            }
        }

    }

    //transposing
    for (j = 0; j < N; j++) {
        for (i = 0; i < N; i++) {
            double temp1;
            temp1 = InMat2[i][j];
            InMat2[i][j] = InMat2[j][i];
            InMat2[j][i] = temp1;
        }
    }



    threads = (pthread_t *) malloc(sizeof(pthread_t) * numberOfThreads);


    /* Distribution of the work. */
    
    timing_start();	//time started  
    currentRow = 0;


    for (i = 0; i < numberOfThreads; i++) {

        pthread_create(&threads[i], NULL, (void *(*)(void *)) operator, (void *) (i + 1));

    }

    for (i = 0; i < numberOfThreads; i++) {

        pthread_join(threads[i], NULL);
    }

    printf("\n\n");


    for (i = 1; i < N; i++) {

        for (j = 0; j < N; j++)

            printf("%lf ", ResMat[i][j]);

        printf("\n");

    }
    timing_stop();        //time stopped
    print_timing();
    get_timing();
    printf("\n We're done!!!!\n");
    char comma[2];

    fp = fopen("outMatrix.csv", "w");  // Saving to output file in csv format
    for (i = 0; i < N; i++) {
        sprintf(comma, "%s", "");
        for (j = 0; j < N; j++) {
            fprintf(fp, "%s%4lf", comma, ResMat[i][j]);
            sprintf(comma, "%s", ",");;
        }
        fprintf(fp, "\n");
    }

    fclose(fp);


}
