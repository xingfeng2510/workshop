#include "vector.h"

#define MAXTHREADS 16
#define NUM 2048

typedef double TYPE;
typedef TYPE array[NUM];

typedef struct {
    int *row_ptr;
    int *col_idx;
    TYPE *values;   
    int num_nonzeros;
} SparseMatrixCSR;

extern void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]);

extern void csrMultiply(int msize, int tidx, int numt, Vector *vec, SparseMatrixCSR *A, SparseMatrixCSR *B, TYPE C[][NUM], TYPE temp[][NUM]);

void Initialize();
void ParallelMultiply(int msize, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM], _Bool isSparse);
void GetModelParams(int *nthreads, int* msize, int print);
