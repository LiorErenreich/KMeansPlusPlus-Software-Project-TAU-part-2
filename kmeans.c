#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int i, j, counter, numberOfLines, cluster, line, row, column, r, max_iter, cord, d, cnt, dim, k;
double* result_oneD;
const int K;
double* m;
double tmp, norma, p;
char ch, c;
char fl[1000] = "";
char* oneFl;
double* x_i;
double* point;
int* Mio_count;
double** result;
double** N;
double** tmp_Mio;
double** mioSums;
double** mioAvr;
double min_j;

double Norma(double* x_i, double* Mio_j, int d) {
    norma = 0.0;
    for (i = 0; i < d; i++) {
        norma += (x_i[i] - Mio_j[i]) * (x_i[i] - Mio_j[i]);
    }
    return norma;
}

/*returns the K centroids after one iteration:
* given result which is the centroids as they are defined at the start of the iteration
* mioSums supposed to be initialized to zeros at the start of this function,
* mioAvr can be accepted with previous values,
*/
double** centroids(double** mioSums, double** mioAvr, double** result, double** N, int* Mio_count, int d, int K, int numberOfLines) {
    /*
    * iterating throghout lines:
    * given a line of doubles, stored as a point:
    * 1. calculates the norn from every centroid in result,
    * 2. updates the minimal norm (min_norm) and the matching closest centroid (min_centroid),
    * 3. addes to matching index in Mio_count and sums the value in every dimention to mioSums
    */
    point = calloc(d, sizeof(double));
    for (line = 0; line < numberOfLines; line++) {
        for (dim = 0; dim < d; dim++) {
            point[dim] = N[line][dim];
        }
        min_j = Norma(point, result[0], d);
        j = 0;

        for (cluster = 0; cluster < K; cluster++)
        {
            tmp = Norma(point, result[cluster], d);
            if (tmp < min_j) {
                min_j = tmp;
                j = cluster;
            }
        }
        Mio_count[j] += 1;

        for (row = 0; row < d; row++) {
            mioSums[j][row] = (mioSums[j][row]) + (point[row]);
        }
    }
    /*
    * recalculate the new centroids:
    * setting to mioAvr the averages of the clusters using the sums and counts,
    * they will be the updated centroids (tmpMio and then result)
     */
    for (cluster = 0; cluster < K; cluster++) {
        for (r = 0; r < d; r++) {
            if (Mio_count[cluster] != 0) {
                mioAvr[cluster][r] = mioSums[cluster][r] / Mio_count[cluster];
            }
            else {
                mioAvr[cluster][r] = result[cluster][r];
                printf("Mio_count[cluster] == 0");
            }
        }
    }
    free((void*)point);
    return (mioAvr);
}


void free_data_2D(double** data, int K)
{
    for (i = 0; i < K; ++i) {
        free((void*)data[i]);
    }
    free((void*)data);
}


void freeptr(void* ptr)
{
    free((void*)ptr);
}

double* cmain(int K, int max_iter, int numberOfLines, int d, double* N_array, double* clusters) {
    N = (double**)malloc(numberOfLines * sizeof(double*));
    for (i = 0; i < numberOfLines; i++) {
        N[i] = (double*)calloc(d, sizeof(double));
    }
    for (i = 0; i < numberOfLines; i++) {
        for (j = 0; j < d; j++) {
            N[i][j] = N_array[d * i + j];
        }
    }
    Mio_count = calloc(K, sizeof(int));
    for (i = 0; i < K; i++) {
        Mio_count[i] = 1;
    }
    result = (double**)malloc(K * sizeof(double*));
    for (i = 0; i < K; i++) {
        result[i] = (double*)calloc(d, sizeof(double));
    }
    mioSums = (double**)malloc(K * sizeof(double*));
    mioAvr = (double**)malloc(K * sizeof(double*));
    for (cluster = 0; cluster < K; cluster++) {
        mioSums[cluster] = (double*)calloc(d, sizeof(double));
        mioAvr[cluster] = (double*)calloc(d, sizeof(double));
    }
    /* sets the first K lines in N to be the first K centroids*/
    for (i = 0; i < K; i++) {
        if (result) {
            for (j = 0; j < d; j++) {
                if (result[i]) {
                    result[i][j] = clusters[i * d + j];
                }
                else {
                    perror("result[i] is NULL when setting the first K clusters");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else {
            perror("result is NULL");
            exit(EXIT_FAILURE);
        }
    }
    counter = 0;
    while (counter < max_iter) {
        free((void*)Mio_count);
        Mio_count = (int*)calloc(K, sizeof(int));
        counter += 1;
        tmp_Mio = centroids(mioSums, mioAvr, result, N, Mio_count, d, K, numberOfLines);
        free_data_2D(mioSums, K);
        mioSums = (double**)malloc(K * sizeof(double*));
        for (cluster = 0; cluster < K; cluster++) {
            mioSums[cluster] = (double*)calloc(d, sizeof(double));
        }
        cnt = 0;
        for (cluster = 0; cluster < K; cluster++) {
            for (row = 0; row < d; row++) {
                if (result[cluster][row] != tmp_Mio[cluster][row]) {
                    cnt++;
                    break;
                }
            }
        }
        if (cnt == 0) {
            break;
        }
        else if (cnt != 0) {
            for (cluster = 0; cluster < K; cluster++) {
                for (row = 0; row < d; row++) {
                    result[cluster][row] = tmp_Mio[cluster][row];
                }
            }
        }
    }

    if (Mio_count) {
        free(Mio_count);
    }
    else {
        perror("Mio_count is NULL");
        exit(EXIT_FAILURE);
    }
    free_data_2D(N, numberOfLines);
    free_data_2D(mioAvr, K);
    result_oneD = calloc(K * d, sizeof(double));
    for (i = 0; i < K; i++) {
        for (j = 0; j < d; j++) {
            result_oneD[i * d + j] = result[i][j];
        }
    }
    free_data_2D(result, K);
    return result_oneD;
}


