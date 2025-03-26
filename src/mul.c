#include "mul.h"
#include <immintrin.h>  // AVXָ�ͷ�ļ�

#define L1_BLOCK 64      // L1����ֿ��С
#define L2_BLOCK 256     // L2����ֿ��С
#define SIMD_WIDTH 4     // AVX2˫�����������

void mul(int msize, int tidx, int numt, Vector *vec, 
         TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]) 
{
    // ��һ����ת�þ���B����ʱ����t�����ƿռ�ֲ��ԣ�
    #pragma omp parallel for
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++) {
            t[j][i] = b[i][j];
        }
    }

    // �ڶ������༶�ֿ�+SIMD����
    for (int i = tidx; i < msize; i += numt) {
        // L2���ֿ�
        for (int jj = 0; jj < msize; jj += L2_BLOCK) {
            for (int kk = 0; kk < msize; kk += L2_BLOCK) {
                // L1���ֿ�
                for (int j = jj; j < jj + L2_BLOCK && j < msize; j += L1_BLOCK) {
                    for (int k = kk; k < kk + L2_BLOCK && k < msize; k += L1_BLOCK) {
                        // SIMD����������
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

        // ����ԭ��vector_append���ã���������ԣ�
        for (int j = 0; j < msize; j++) {
            vector_append(vec, c[i][j]);
        }
    }
}