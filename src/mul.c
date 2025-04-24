#include <immintrin.h>
#include "mul.h"


void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
    int i,j,k;
    const int VEC_LEN = 8; // AVX512 处理 8 个 double
    
    for (int kk = 0; kk < msize; kk += BLOCK_SIZE) {  // K维度分块
        for (int jj = 0; jj < msize; jj += BLOCK_SIZE) {  // J维度分块
            // 块内处理
            for (int k = kk; k < kk + BLOCK_SIZE && k < msize; k++) {
                for (int i = tidx; i < msize; i += numt) {
                    __m512d a_val = _mm512_set1_pd(a[i][k]);
                    
                    // 主向量化部分
                    for (int j = jj; j < jj + BLOCK_SIZE - (VEC_LEN-1); j += VEC_LEN) {
                        if(j >= msize) break;
                        __m512d b_vec = _mm512_load_pd(&b[k][j]);
                        __m512d c_vec = _mm512_load_pd(&c[i][j]);
                        c_vec = _mm512_fmadd_pd(a_val, b_vec, c_vec);
                        _mm512_store_pd(&c[i][j], c_vec);
                    }
                    
                    // 处理块尾部
                    for (int j = jj + BLOCK_SIZE - (BLOCK_SIZE%VEC_LEN); 
                        j < jj + BLOCK_SIZE && j < msize; j++) {
                        c[i][j] += a[i][k] * b[k][j];
                    }
                }
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