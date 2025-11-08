// matrix.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20          // matrix dimension (20 x 20)
#define NUM_THREADS 10  // run 10 threads in parallel

int matA[MAX][MAX];
int matB[MAX][MAX];

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];

typedef struct {
    int start_row;  // inclusive
    int end_row;    // exclusive
} Slice;

static void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

static void printMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Sum A and B into matSumResult for rows [start_row, end_row)
static void* computeSum(void* args) {
    Slice* s = (Slice*)args;
    for (int i = s->start_row; i < s->end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }
    free(s);
    return NULL;
}

// Difference A - B into matDiffResult for rows [start_row, end_row)
static void* computeDiff(void* args) {
    Slice* s = (Slice*)args;
    for (int i = s->start_row; i < s->end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }
    free(s);
    return NULL;
}

// Product A x B into matProductResult for rows [start_row, end_row)
static void* computeProduct(void* args) {
    Slice* s = (Slice*)args;
    for (int i = s->start_row; i < s->end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            int sum = 0;
            for (int k = 0; k < MAX; k++) {
                sum += matA[i][k] * matB[k][j];
            }
            matProductResult[i][j] = sum;
        }
    }
    free(s);
    return NULL;
}

// helper to launch a batch of threads over row slices for a given worker
static void run_in_parallel(void* (*worker)(void*)) {
    pthread_t threads[NUM_THREADS];

    int base = MAX / NUM_THREADS;   // rows per thread
    int extra = MAX % NUM_THREADS;  // first 'extra' threads get one more row

    int next_row = 0;
    for (int t = 0; t < NUM_THREADS; t++) {
        int take = base + (t < extra ? 1 : 0);
        Slice* s = (Slice*)malloc(sizeof(Slice));
        s->start_row = next_row;
        s->end_row = next_row + take;
        next_row = s->end_row;

        pthread_create(&threads[t], NULL, worker, s);
    }
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
}

int main(int argc, char* argv[]) {
    srand((unsigned)time(NULL));  // leave this in

    // If a size is provided, warn if it does not match 20
    if (argc >= 2) {
        int wanted = atoi(argv[1]);
        if (wanted != MAX) {
            fprintf(stderr, "Note: this program is fixed to %dx%d. Ignoring argv[1]=%d.\n",
                    MAX, MAX, wanted);
        }
    }

    // 1. Fill the matrices with random values
    fillMatrix(matA);
    fillMatrix(matB);

    // 2. Print the initial matrices
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // 3 and 4. Create and run 10 threads for each operation
    run_in_parallel(&computeSum);
    run_in_parallel(&computeDiff);
    run_in_parallel(&computeProduct);

    // 6. Print the results
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}
