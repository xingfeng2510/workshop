#include "mul.h"
#include <immintrin.h>  // AVX指令集头文件

#define L1_BLOCK 64      // L1缓存分块大小
#define L2_BLOCK 256     // L2缓存分块大小
#define SIMD_WIDTH 4     // AVX2双精度向量宽度

void mul(int msize, int tidx, int numt, Vector *vec, 
         TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]) 
{
    // 第一步：转置矩阵B到临时矩阵t（改善空间局部性）
    #pragma omp parallel for
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++) {
            t[j][i] = b[i][j];
        }
    }

    // 第二步：多级分块+SIMD计算
    for (int i = tidx; i < msize; i += numt) {
        // L2级分块
        for (int jj = 0; jj < msize; jj += L2_BLOCK) {
            for (int kk = 0; kk < msize; kk += L2_BLOCK) {
                // L1级分块
                for (int j = jj; j < jj + L2_BLOCK && j < msize; j += L1_BLOCK) {
                    for (int k = kk; k < kk + L2_BLOCK && k < msize; k += L1_BLOCK) {
                        // SIMD向量化计算
                        for (int sub_j = j; sub_j < j + L1_BLOCK && sub_j < msize; sub_j += SIMD_WIDTH) {
                            __m256d c_vec = _mm256_loadu_pd(&c[i][sub_j]);
                            
                            for (int sub_k = k; sub_k < k + L1_BLOCK && sub_k < msize; sub_k++) {
                                __m256d a_val = _mm256_set1_pd(a[i][sub_k]);
                                __m256d b_vec = _mm256_loadu_pd(&t[sub_j][sub_k]);
                                c_vec = _mm256_fmadd_pd(a_val, b_vec, c_vec);
                            }
                            
                            _mm256_storeu_pd(&c[i][sub_j], c_vec);
                        }
                    }
                }
            }
        }

        // 保持原有vector_append调用（如需兼容性）
        for (int j = 0; j < msize; j++) {
            vector_append(vec, c[i][j]);
        }
    }
}