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

    for (int i = start; i < end; i++) {
        for (int j = 0; j < msize; j++) {
            TYPE acc = c[i][j];
            
            // Parallize for-k (assume data is aligned)
            // NOTE: to compile this we should add -march=native. Fortunately, avx512 is supported.
            int k = 0;
            __m512d acc_vec = _mm512_setzero_pd(); // 8 * sizeof(double)
            int k_limit = msize - (msize % 8);
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
            for (; k < msize; k++) {
                acc += a[i][k] * t[j][k];
            }

            c[i][j] = acc;
            // vector_append appends an int cast from a TYPE(double).
            // vector_append(vec, acc);
            vec->data[vec->size++] = acc;
        }
    }
}
