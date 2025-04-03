#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "mul.h"
#include "vector.h"

#define xstr(s) x_str(s)
#define x_str(s) #s

typedef struct tparam
{
	array *a, *b, *c, *t;
	int msize;
	int tidx;
	int numt;
	Vector *vec;
} _tparam;

void *ThreadFunction(void *ptr)
{
	_tparam* par = (_tparam*)ptr;
	assert(par->numt > 0);
	assert(par->a != NULL);
	assert(par->b != NULL);
	assert(par->c != NULL);
	assert(par->t != NULL);
	assert( (par->msize % par->numt) == 0);

	mul(par->msize, par->tidx, par->numt, par->vec, par->a, par->b, par->c, par->t);

	// destroy vector here
	vector_destroy(par->vec);

	pthread_exit((void *)0);
}


void ParallelMultiply(int msize, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM])
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
	// NTHREADS = 1;

	for (tidx = 0; tidx < NTHREADS; tidx++) {
		par[tidx].msize = MSIZE;
		par[tidx].numt = NTHREADS;
		par[tidx].tidx = tidx;
		par[tidx].vec = vector_create();
		par[tidx].a = a;
		par[tidx].b = b;
		par[tidx].c = c;
		par[tidx].t = t;
		tret[tidx] = pthread_create(&ht[tidx], NULL, (void*)ThreadFunction, (void*) &par[tidx]);
	}

	for (tidx = 0; tidx < NTHREADS; tidx++) {
		rc = pthread_join(ht[tidx], (void **)&status);
	}
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
