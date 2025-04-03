#include "mul.h"

void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]) {
    int i, j, k;
    for (i = tidx; i < msize; i += numt) {
        // 初始化c[i][j]为0
        for (j = 0; j < msize; j++) {
            c[i][j] = 0.0;
        }
        // 调整循环顺序为i-k-j
        for (k = 0; k < msize; k++) {
            // 缓存a[i][k]减少重复访问
            TYPE a_ik = a[i][k]; 
            for (j = 0; j < msize; j++) {
                c[i][j] += a_ik * b[k][j];
            }
        }
    }
}
