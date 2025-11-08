#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20
#define NUM_THREADS 10

int matA[MAX][MAX];
int matB[MAX][MAX];
int matSumResult[MAX][MAX];

typedef struct {
    int start_row;
    int end_row;
} Slice;

void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++)
        for (int j = 0; j < MAX; j++)
            matrix[i][j] = rand() % 10 + 1;
}

void printMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++)
            printf("%5d", matrix[i][j]);
        printf("\n");
    }
    printf("\n");
}

void* computeSum(void* args) {
    Slice* s = (Slice*)args;
    for (int i = s->start_row; i < s->end_row; i++)
        for (int j = 0; j < MAX; j++)
            matSumResult[i][j] = matA[i][j] + matB[i][j];
    free(s);
    return NULL;
}

int main() {
    srand(time(0));

    fillMatrix(matA);
    fillMatrix(matB);

    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    pthread_t threads[NUM_THREADS];
    int rows_per_thread = MAX / NUM_THREADS;

    for (int t = 0; t < NUM_THREADS; t++) {
        Slice* s = malloc(sizeof(Slice));
        s->start_row = t * rows_per_thread;
        s->end_row = (t == NUM_THREADS - 1) ? MAX : (t + 1) * rows_per_thread;
        pthread_create(&threads[t], NULL, computeSum, s);
    }

    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(threads[t], NULL);

    printf("Sum Result:\n");
    printMatrix(matSumResult);
    return 0;
}
