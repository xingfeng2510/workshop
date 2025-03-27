#include "mul.h"
#include <string.h>
#define BLOCK_SIZE 64

#define min(a, b) ((a) < (b) ? (a) : (b))

void csrMultiply(int msize, int tidx, int numt, Vector *vec, SparseMatrixCSR *A, SparseMatrixCSR *B, TYPE C[][NUM], TYPE temp[][NUM]){
	for (int block_i = tidx * BLOCK_SIZE; block_i < msize; block_i += numt * BLOCK_SIZE) {
        int block_end = (block_i + BLOCK_SIZE < msize) ? block_i + BLOCK_SIZE : msize;
        for (int i = block_i; i < block_end; i++) {
            TYPE local_temp[NUM] = {0};
            for (int j = A->row_ptr[i]; j < A->row_ptr[i + 1]; j++) {
                int a_col = A->col_idx[j];
                TYPE a_val = A->values[j];
                for (int k = B->row_ptr[a_col]; k < B->row_ptr[a_col + 1]; k++) {
                    int b_col = B->col_idx[k];
                    TYPE b_val = B->values[k];
                    local_temp[b_col] += a_val * b_val;
                }
            }
            for (int col = 0; col < NUM; col++) {
                C[i][col] += local_temp[col];
            }
        }
    }
}

void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
{
	for (int bi = tidx * BLOCK_SIZE; bi < msize; bi += numt * BLOCK_SIZE) {
        for (int bk = 0; bk < msize; bk += BLOCK_SIZE) {
            for (int bj = 0; bj < msize; bj += BLOCK_SIZE) {
                int i_end = min(bi + BLOCK_SIZE, msize);
                int k_end = min(bk + BLOCK_SIZE, msize);
                int j_end = min(bj + BLOCK_SIZE, msize);

                for (int i = bi; i < i_end; i++) {
                    for (int k = bk; k < k_end; k++) {
                        TYPE a_ik = a[i][k];
                        #pragma omp simd
                        for (int j = bj; j < j_end; j++) {
                            c[i][j] += a_ik * b[k][j];
                        }
                    }
                }
            }
        }
    }
}
