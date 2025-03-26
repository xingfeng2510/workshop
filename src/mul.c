#include "mul.h"

#define BLOCK_SIZE 64  // 根据L1缓存大小选择

void mul(int msize, int tidx, int numt, Vector *vec, 
         TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
    int i, j, k, ii, jj, kk;
    
    for (i = tidx; i < msize; i += numt) {
        for (j = 0; j < msize; j++) {
            c[i][j] = 0;  // 显式初始化
        }
    }

    for (i = tidx; i < msize; i += numt) {
        for (jj = 0; jj < msize; jj += BLOCK_SIZE) {
            for (kk = 0; kk < msize; kk += BLOCK_SIZE) {
                for (ii = i; ii < i + numt && ii < msize; ii++) {
                    for (j = jj; j < jj + BLOCK_SIZE && j < msize; j++) {
                        TYPE sum = c[ii][j];
                        for (k = kk; k < kk + BLOCK_SIZE && k < msize; k++) {
                            sum += a[ii][k] * b[k][j];
                        }
                        c[ii][j] = sum;
                    }
                }
            }
        }
    }
}
