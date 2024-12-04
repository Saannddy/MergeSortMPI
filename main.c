// Parallel Mergesort MPI Project
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int nsize[] = {10000, 100000, 500000, 1000000, 2000000};
    int numsize = sizeof(nsize) / sizeof(nsize[0]), s;
    
    for(s = 0; s < numsize; s++){
        int n = nsize[s];
        int *arr = NULL, local_n, i;

        if(rank == 0) {
            arr = malloc(n * sizeof(int));
            srand(629180);
            for (i = 0; i < n; i++) arr[i] = rand() % 100000;
        }

        local_n = n / size;
        int *local_arr = malloc(local_n * sizeof(int));
        double start = MPI_Wtime();
	    MPI_Scatter(arr, local_n, MPI_INT, local_arr, local_n, MPI_INT, 0, MPI_COMM_WORLD); 

        mergeSort(local_arr, 0, local_n - 1); //we don't need threshold since our performance is already good

        MPI_Gather(local_arr, local_n, MPI_INT, arr, local_n, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            double end = MPI_Wtime();
            mergeSort(arr, 0, n - 1);
            printf("Sorting completed for n = %d in %f seconds with %d nodes\n", n, end - start, size);
            //for(i=0;i<n-1;i++) printf("%d ", arr[i]); //Uncomment to see the sorted array
	    free(arr);
        }
        free(local_arr); 

    }

    MPI_Finalize();
    return 0;
}

void merge(int arr[], int l, int m, int r){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    int *L = malloc(n1 * sizeof(int));
    int *R = malloc(n2 * sizeof(int));

    for(i = 0; i < n1; i++) L[i] = arr[l + i];
    for(j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;
    while(i < n1 && j < n2) arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while(i < n1) arr[k++] = L[i++];
    while(j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(int arr[], int l, int r){
    if(l < r){
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}
