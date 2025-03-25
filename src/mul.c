#include "mul.h"
#include <omp.h>
#include <immintrin.h>
#include <stdlib.h>

#define BLOCK_SIZE 64 // 设定 Block Matrix 计算块大小

void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]) 
{
    int i, j, k, ii, jj, kk;
    
    omp_set_num_threads(4); // 避免 OpenMP 过度线程化

    // **使用块矩阵算法**
    #pragma omp parallel for private(ii, jj, kk, i, j, k) shared(a, b, c) schedule(dynamic)
    for (ii = 0; ii < msize; ii += BLOCK_SIZE) {
        for (jj = 0; jj < msize; jj += BLOCK_SIZE) {
            for (kk = 0; kk < msize; kk += BLOCK_SIZE) {
                for (i = ii; i < ii + BLOCK_SIZE && i < msize; i++) {
                    for (j = jj; j < jj + BLOCK_SIZE && j < msize; j++) {
                        __m256d sum = _mm256_setzero_pd();
                        for (k = kk; k < kk + BLOCK_SIZE && k < msize; k += 4) {
                            __m256d va = _mm256_load_pd(&a[i][k]);
                            __m256d vb = _mm256_load_pd(&b[k][j]);
                            sum = _mm256_fmadd_pd(va, vb, sum);
                        }
                        double sum_arr[4];
                        _mm256_store_pd(sum_arr, sum);
                        c[i][j] += sum_arr[0] + sum_arr[1] + sum_arr[2] + sum_arr[3];
                    }
                }
            }
        }
    }
}
