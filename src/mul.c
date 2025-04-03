#include "mul.h"
#include <stdlib.h>
#include <immintrin.h>

__attribute__((optimize("unroll-loops")))
void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
    // avoid alternative access (nearly helps)
    int stride = msize / numt;
    int last_thread = tidx == (numt - 1);
    int start = tidx * stride;
    int end = last_thread ? msize : ((tidx + 1) * stride);

    // pre-allocate buffer (nearly helps)
    int numel_this_thread = (end - start) * msize;
    if (vec->capacity < vec->size + numel_this_thread) {
        vec->capacity = vec->size + numel_this_thread;
        vec->data = realloc(vec->data, vec->capacity * sizeof(int));
    }

    // The original code performs a matrix-multiply-add, and we keep the semantics here by loading
    // the initial value of c[i][j].

    // 1. 2048 / sizeof(double) = 256 elements should be multiple of any block size.
    // 2. block sizes should be multiple of 512/64 = 8 elements.
    const int BLOCK_SIZE_I = 128;
    const int BLOCK_SIZE_J = 128;
    const int BLOCK_SIZE_K = 128;

    for (int ii = start; ii < end; ii += BLOCK_SIZE_I) {
        int i_end = (ii + BLOCK_SIZE_I < end) ? ii + BLOCK_SIZE_I : end;
        
        for (int jj = 0; jj < msize; jj += BLOCK_SIZE_J) {
            int j_end = (jj + BLOCK_SIZE_J < msize) ? jj + BLOCK_SIZE_J : msize;
            
            for (int kk = 0; kk < msize; kk += BLOCK_SIZE_K) {
                int k_end = (kk + BLOCK_SIZE_K < msize) ? kk + BLOCK_SIZE_K : msize;

                for (int i = ii; i < i_end; i++) {
                    for (int j = jj; j < j_end; j++) {
                        TYPE acc = c[i][j];
                        
                        // Parallize for-k (assume data is aligned)
                        // NOTE: to compile this we should add -march=native. Fortunately, avx512 is supported.
                        int k = kk;
                        __m512d acc_vec = _mm512_setzero_pd(); // 8 * sizeof(double)
                        int k_limit = k_end - ((k_end - kk) % 8);
                        for (; k < k_limit; k += 8) {
                            __m512d a_vec = _mm512_loadu_pd(&a[i][k]);
                            __m512d b_vec = _mm512_loadu_pd(&t[j][k]);
                            acc_vec = _mm512_fmadd_pd(a_vec, b_vec, acc_vec);
                        }
                        __m256d sum_vec256 = _mm256_add_pd(_mm512_castpd512_pd256(acc_vec),
                                                        _mm512_extractf64x4_pd(acc_vec, 1));
                        __m128d vlow  = _mm256_castpd256_pd128(sum_vec256);
                        __m128d vhigh = _mm256_extractf128_pd(sum_vec256, 1);
                        vlow  = _mm_add_pd(vlow, vhigh);
                        __m128d high64 = _mm_unpackhi_pd(vlow, vlow);
                        __m128d sum_sd = _mm_add_sd(vlow, high64);
                        acc += _mm_cvtsd_f64(sum_sd);
                        for (; k < k_end; k++) {
                            acc += a[i][k] * t[j][k];
                        }

                        c[i][j] = acc;
                    }
                }
            }
        }
    }

    for (int i = start; i < end; i++) {
        for (int j = 0; j < msize; j++) {
            vec->data[vec->size++] = c[i][j];
        }
    }
}
