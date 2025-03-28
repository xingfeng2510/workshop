#include "mul.h"
#include <immintrin.h>  // AVX2ָ�
#include <stdint.h>     // ��ȷʹ��int32_t

#define L1_BLOCK 64      // L1����ֿ飨����int32_t��
#define L2_BLOCK 256     // L2����ֿ�
#define SIMD_WIDTH 8     // AVX2�ɴ���8��int32_t

void mul(int msize, int tidx, int numt, Vector *vec, 
         TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]) 
{
    // 1. ת�þ���B�����ƿռ�ֲ��ԣ�
    #pragma omp parallel for
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++) {
            t[j][i] = b[i][j];
        }
    }

    // 2. �༶�ֿ�
    for (int i = tidx; i < msize; i += numt) {
        for (int jj = 0; jj < msize; jj += L2_BLOCK) {
            for (int kk = 0; kk < msize; kk += L2_BLOCK) {
                // L1���ֿ�
                for (int j = jj; j < jj + L2_BLOCK && j < msize; j += L1_BLOCK) {
                    for (int k = kk; k < kk + L2_BLOCK && k < msize; k += L1_BLOCK) {
                        // 3. AVX2����ָ�ÿ�ִ���8��int32_t��SIMD_WIDTH=8��
                        for (int sub_j = j; sub_j < j + L1_BLOCK && sub_j < msize; sub_j += SIMD_WIDTH) {
                            __m256i c_vec = _mm256_load_si256((__m256i*)&c[i][sub_j]); // �������
                            
                            for (int sub_k = k; sub_k < k + L1_BLOCK && sub_k < msize; sub_k++) {
                                __m256i a_val = _mm256_set1_epi32(a[i][sub_k]);
                                __m256i b_vec = _mm256_load_si256((__m256i*)&t[sub_j][sub_k]); // �������
                                // �����˼ӣ�c_vec += a_val * b_vec
                                __m256i prod = _mm256_mullo_epi32(a_val, b_vec);
                                c_vec = _mm256_add_epi32(c_vec, prod);
                            }
                            
                            _mm256_store_si256((__m256i*)&c[i][sub_j], c_vec); // ����洢
                        }
                    }
                }
            }
        }

        // ����ԭ��vector_append����
        for (int j = 0; j < msize; j++) {
            vector_append(vec, c[i][j]);
        }
    }
}