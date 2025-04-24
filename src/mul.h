#include "vector.h"

#define MAXTHREADS 2
#define NUM 2048
#define BLOCK_SIZE 1024

typedef double TYPE;
typedef TYPE array[NUM];

extern void mul(int msize, int tidx, int numt, Vector *vec, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]);

void Initialize();
void ParallelMultiply(int msize, TYPE a[][NUM], TYPE b[][NUM], TYPE c[][NUM], TYPE t[][NUM]);
void GetModelParams(int *nthreads, int* msize, int print);
