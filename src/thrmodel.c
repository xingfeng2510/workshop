#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "mul.h"
#include "vector.h"

#define xstr(s) x_str(s)
#define x_str(s) #s
#define SPARSE_THRESHOLD 0.1


typedef struct tparam
{
	array *a, *b, *c, *t;
	int msize;
	int tidx;
	int numt;
	Vector *vec;
	SparseMatrixCSR *sparseA;
        SparseMatrixCSR	*sparseB;
	_Bool isSparse;
} _tparam;

int isSparse(TYPE mat[][NUM], int msize) {
    int nonZero = 0, total = msize * msize;
    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++) {
            if (mat[i][j] != 0) nonZero++;
            if ((double)nonZero / total > SPARSE_THRESHOLD) return 0; 
        }
    }
    return 1; 
}

void convertToCSR(TYPE mat[][NUM], int msize, SparseMatrixCSR *csrMat) {
    int nnz = 0;

    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++) {
            if (mat[i][j] != 0) {
                nnz++;
            }
        }
    }
    csrMat->num_nonzeros = nnz;
    csrMat->values = (TYPE *)malloc(nnz * sizeof(TYPE));
    csrMat->col_idx = (int *)malloc(nnz * sizeof(int));
    csrMat->row_ptr = (int *)malloc((msize + 1) * sizeof(int));

    int pos = 0;
    csrMat->row_ptr[0] = 0; 

    for (int i = 0; i < msize; i++) {
        for (int j = 0; j < msize; j++) {
            if (mat[i][j] != 0) {
                csrMat->values[pos] = mat[i][j];
                csrMat->col_idx[pos] = j;  
                pos++;
            }
        }
        csrMat->row_ptr[i + 1] = pos; 
    }
}

void freeCSR(SparseMatrixCSR *csrMat) {
    free(csrMat->values);
    free(csrMat->col_idx);
    free(csrMat->row_ptr);

    csrMat->values = NULL;
    csrMat->col_idx = NULL;
    csrMat->row_ptr = NULL;
}

void *ThreadFunction(void *ptr)
{
	_tparam* par = (_tparam*)ptr;
	assert(par->numt > 0);
	assert(par->a != NULL);
	assert(par->b != NULL);
	assert(par->c != NULL);
	assert(par->t != NULL);
	assert( (par->msize % par->numt) == 0);
	if (par->isSparse) {
		//printf("sparse mutiply\n");
		csrMultiply(par->msize, par->tidx, par->numt, par->vec, par->sparseA, par->sparseB, par->c, par->t);
	}else{
	    //printf("dense mutiply\n");
	    mul(par->msize, par->tidx, par->numt, par->vec, par->a, par->b, par->c, par->t);
	}
	pthread_exit((void *)0);
}


void ParallelMultiply(int msize, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM], _Bool isSparse)
{
	int NTHREADS = MAXTHREADS;
	int MSIZE = NUM;

	pthread_t ht[MAXTHREADS];
	int tret[MAXTHREADS]; 
	int rc; 
	void* status;
	_tparam par[MAXTHREADS];
	int tidx;

	GetModelParams(&NTHREADS, &MSIZE, 0);

	SparseMatrixCSR A, B;
        convertToCSR(a, MSIZE, &A);
        convertToCSR(b, MSIZE, &B);
	for (tidx = 0; tidx < NTHREADS; tidx++) {
		par[tidx].msize = MSIZE;
		par[tidx].numt = NTHREADS;
		par[tidx].tidx = tidx;
		par[tidx].vec = vector_create();
		par[tidx].sparseA = &A;
		par[tidx].sparseB = &B;
		par[tidx].a = a;
		par[tidx].b = b;
		par[tidx].c = c;
		par[tidx].t = t;
		par[tidx].isSparse = isSparse;
		tret[tidx] = pthread_create(&ht[tidx], NULL, (void*)ThreadFunction, (void*) &par[tidx]);
	}

	for (tidx = 0; tidx < NTHREADS; tidx++) {
		rc = pthread_join(ht[tidx], (void **)&status);
	}
	freeCSR(&A);
	freeCSR(&B);
}

extern int getCPUCount();

void GetModelParams(int* p_nthreads, int* p_msize, int print)
{
	int nthr = MAXTHREADS;
	int msize = NUM;
	int ncpu = getCPUCount();

	if (ncpu < MAXTHREADS) {
		nthr = ncpu;
	}

	// Making sure the matrix size and the nthreads are aligned
	// If you want more robust threading implementation, take care
	// of the matrix tails
	while ((msize % nthr) != 0 )
		nthr--;

	if(p_nthreads != 0)
		*p_nthreads = nthr;
	if(p_msize != 0)
		*p_msize = msize;

	if(print) {
		printf("Threads: %d\n", nthr);
		fflush(stdout);
		printf("Matrix size: %d\n",msize);
		fflush(stdout);
	}
}
