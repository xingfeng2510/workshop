#include <immintrin.h>
#include "mul.h"


void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
    int i,j,k;
    const int VEC_LEN = 8; // AVX512 处理 8 个 double
    
    for (k = 0; k < msize; k++) {
        for (i = tidx; i < msize; i += numt) {
            __m512d a_vec = _mm512_set1_pd(a[i][k]); // 广播标量值
            
            // 主循环处理向量化部分
            for (j = 0; j < msize - (VEC_LEN-1); j += VEC_LEN) {
                __m512d b_vec = _mm512_load_pd(&b[k][j]);     // 对齐加载
                __m512d c_vec = _mm512_load_pd(&c[i][j]);
                c_vec = _mm512_fmadd_pd(a_vec, b_vec, c_vec);
                _mm512_store_pd(&c[i][j], c_vec);            // 对齐存储
            }

            // 处理尾部元素（不足8个）
            for (; j < msize; j++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    
    // 后处理vector_append
    for (i = tidx; i < msize; i += numt) {
        for (j = 0; j < msize; j++) {
            vector_append(vec, c[i][j]);
        }
    }
}