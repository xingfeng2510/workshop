#include "mul.h"

void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
    int i,j,k;
    
    for (k = 0; k < msize; k++) {
        for (i = tidx; i < msize; i += numt) {
            TYPE a_val = a[i][k];
            for (j = 0; j < msize; j++) {
                c[i][j] += a_val * b[k][j];
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