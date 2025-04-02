#include "mul.h"

void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]) {
    int i, j, k;
    for (i = tidx; i < msize; i += numt) {
        // ��ʼ��c[i][j]Ϊ0
        for (j = 0; j < msize; j++) {
            c[i][j] = 0.0;
        }
        // ����ѭ��˳��Ϊi-k-j
        for (k = 0; k < msize; k++) {
            // ����a[i][k]�����ظ�����
            TYPE a_ik = a[i][k]; 
            for (j = 0; j < msize; j++) {
                c[i][j] += a_ik * b[k][j];
            }
        }
    }
}
