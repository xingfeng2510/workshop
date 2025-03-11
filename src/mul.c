#include "mul.h"
#define min(a,b) ((a) < (b) ? (a) : (b))

void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
    int i, j, k, jj, kk;
    int blockSize = 8;  // 块大小

    for (i = tidx; i < msize; i += numt) {
        for (j = 0; j < msize; j++) {
            c[i][j] = 0;
        }
        // 对 k 和 j 两个维度进行分块处理
        for (kk = 0; kk < msize; kk += blockSize) {
            int k_max = min(kk + blockSize, msize);
            for (jj = 0; jj < msize; jj += blockSize) {
                int j_max = min(jj + blockSize, msize);
                for (k = kk; k < k_max; k++) {
                    // 为了提高效率，可先取出 a[i][k]
                    TYPE a_val = a[i][k];
                    for (j = jj; j < j_max; j++) {
                        c[i][j] += a_val * b[k][j];
                    }
                }
            }
        }
        for (j = 0; j < msize; j++) {
            vector_append(vec, c[i][j]);
        }
    }
}
