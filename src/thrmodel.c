#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "mul.h"
#include "vector.h"

typedef struct tparam {
    array *a, *b, *c, *t;
    int msize;
    int tidx;
    int numt;
    Vector *vec;
} tparam;

void *ThreadFunction(void *ptr) {
    tparam* par = (tparam*)ptr;
    assert(par->numt > 0);
    assert(par->a != NULL && par->b != NULL && par->c != NULL && par->t != NULL);
    assert((par->msize % par->numt) == 0);

    mul(par->msize, par->tidx, par->numt, par->vec, par->a, par->b, par->c, par->t);
    return NULL;
}

void ParallelMultiply(int msize, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]) {
    int NTHREADS = MAXTHREADS;
    int MSIZE = NUM;

    GetModelParams(&NTHREADS, &MSIZE, 0);

    pthread_t *threads = malloc(sizeof(pthread_t) * NTHREADS);
    tparam *params = malloc(sizeof(tparam) * NTHREADS);
    if (!threads || !params) {
        fprintf(stderr, "内存分配失败\n");
        exit(EXIT_FAILURE);
    }

    for (int tidx = 0; tidx < NTHREADS; tidx++) {
        params[tidx].msize = MSIZE;
        params[tidx].numt = NTHREADS;
        params[tidx].tidx = tidx;
        params[tidx].vec = vector_create();
        params[tidx].a = a;
        params[tidx].b = b;
        params[tidx].c = c;
        params[tidx].t = t;
        int rc = pthread_create(&threads[tidx], NULL, ThreadFunction, (void*)&params[tidx]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create 出错，错误码：%d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    for (int tidx = 0; tidx < NTHREADS; tidx++) {
        int rc = pthread_join(threads[tidx], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join 出错，错误码：%d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    free(threads);
    free(params);
}

extern int getCPUCount();

void GetModelParams(int* p_nthreads, int* p_msize, int print) {
    int nthr = MAXTHREADS;
    int msize = NUM;
    int ncpu = getCPUCount();

    if (ncpu < nthr)
        nthr = ncpu;

    while ((msize % nthr) != 0 && nthr > 1) {
        nthr--;
    }

    if (p_nthreads)
        *p_nthreads = nthr;
    if (p_msize)
        *p_msize = msize;

    if (print) {
        printf("Threads: %d\n", nthr);
        printf("Matrix size: %d\n", msize);
        fflush(stdout);
    }
}
