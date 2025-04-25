#include <immintrin.h>
#include "mul.h"
#include <stdint.h>


void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
    const int VEC_LEN = 8; // AVX512 处理 8 个 double
    
    // 线程任务划分（连续i访问）
    int stride = msize / numt;
    int end = tidx == (numt - 1) ? msize : ((tidx + 1) * stride);

    // 三阶分块（i j k 分块顺序）
    for (int ii = tidx * stride; ii < end; ii += BLOCK_SIZE) {
        int i_end = (ii + BLOCK_SIZE < end) ? ii + BLOCK_SIZE : end;
        
        for (int jj = 0; jj < msize; jj += BLOCK_SIZE) {
            int j_end = (jj + BLOCK_SIZE < msize) ? jj + BLOCK_SIZE : msize;
            
            for (int kk = 0; kk < msize; kk += BLOCK_SIZE) {
                int k_end = (kk + BLOCK_SIZE < msize) ? kk + BLOCK_SIZE : msize;

                // 块内处理（k维度向量化）
                for (int i = ii; i < i_end; i++) {
                    for (int j = jj; j < j_end; j++) {
                        TYPE acc = c[i][j];  // 初始化累加器，利用 register
                        
                        // 向量化k循环（利用t的连续访问）
                        __m512d acc_vec = _mm512_setzero_pd();
                        int k = kk;
                        for (; k <  k_end - ((k_end - kk) % VEC_LEN); k += VEC_LEN) {
                            __m512d a_vec = _mm512_loadu_pd(&a[i][k]);
                            __m512d t_vec = _mm512_loadu_pd(&t[j][k]); // t[j][k]连续访问
                            acc_vec = _mm512_fmadd_pd(a_vec, t_vec, acc_vec);
                        }
                        
                        // 水平相加 减少内存写入
                        acc += _mm512_reduce_add_pd(acc_vec);
                        
                        // 处理尾部k元素
                        for (; k < k_end; k++) {
                            acc += a[i][k] * t[j][k];
                        }
                        
                        c[i][j] = acc;  // 单次写回
                    }
                }
            }
        }
    }
    
    
    
    // 后处理vector_append
    for (int i = tidx; i < msize; i += numt) {
        for (int j = 0; j < msize; j++) {
            vector_append(vec, c[i][j]);
        }
    }
}