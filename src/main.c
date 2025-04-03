#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>

#include "mul.h"

typedef unsigned long long  UINT64;

double TRIP_COUNT = (double)NUM * (double)NUM * (double)NUM;
int FLOP_PER_ITERATION = 2;

extern int getCPUCount();
extern double getCPUFreq();

// Initialize an array with data
void init_arr(TYPE row, TYPE col, TYPE off, TYPE a[][NUM])
{
    int i,j;

    for (i = 0; i < NUM; i++) {
        for (j = 0; j< NUM; j++) {
            a[i][j] = row * i + col * j + off;
        }
    }
}

// Print out contents of small arrays
void print_arr(char *name, TYPE array[][NUM])
{
    int i,j;
  
    printf("\n%s\n", name);
    for (i = 0; i < NUM; i++){
        for (j = 0; j < NUM; j++) {
            printf("%g\t", array[i][j]);
        }
        printf("\n");
        fflush(stdout);
    }
}

void MultiplyOnce(int iter, bool do_check)
{
    double start=0.0, stop=0.0;
    struct timeval  before, after;
    double secs;
   	double flops;
	double mflops;

	char *buf1, *buf2, *buf3, *buf4;
	char *addr1, *addr2, *addr3, *addr4;
	array *a, *b, *c, *t;
	int Offset_Addr1 = 128, Offset_Addr2 = 192, Offset_Addr3 = 0, Offset_Addr4 = 64;

	buf1 = (char *) malloc(NUM*NUM*(sizeof (double))+1024);
	addr1 = buf1 + 256 - ((UINT64)buf1%256) + (UINT64)Offset_Addr1;
	
	buf2 = (char *) malloc(NUM*NUM*(sizeof (double))+1024);
	addr2 = buf2 + 256 - ((UINT64)buf2%256) + (UINT64)Offset_Addr2;
	
	buf3 = (char *) malloc(NUM*NUM*(sizeof (double))+1024);
	addr3 = buf3 + 256 - ((UINT64)buf3%256) + (UINT64)Offset_Addr3;

	buf4 = (char *) malloc(NUM*NUM*(sizeof (double))+1024);
	addr4 = buf4 + 256 - ((UINT64)buf4%256) + (UINT64)Offset_Addr4;

	int nbytes = NUM*NUM*(sizeof (double))+1024;
	memset(buf1, 0, nbytes);
	memset(buf2, 0, nbytes);
	memset(buf3, 0, nbytes);
	memset(buf4, 0, nbytes);

	a = (array *)addr1;
	b = (array *)addr2;
	c = (array *)addr3;
	t = (array *)addr4;

    // initialize the arrays with data
	init_arr(3, -2, 1, a);
	init_arr(-2, 1, 3, b);
	init_arr(1, 3, -2, t);
	
	gettimeofday(&before, NULL);

	for (int k = 0; k < NUM; k++) {
        for (int j = 0; j < NUM; j++) {
            t[j][k] = b[k][j];
        }
    }

	ParallelMultiply(NUM, a, b, c, t);

	gettimeofday(&after, NULL);
	secs = (after.tv_sec - before.tv_sec) + (after.tv_usec - before.tv_usec)/1000000.0;

	flops = TRIP_COUNT * FLOP_PER_ITERATION;
	mflops = flops / 1000000.0f / secs;
	printf("Matrix multiply iteration %d: cost %2.3lf seconds\n", iter, secs);
	fflush(stdout);

	// Also, check if mul is correct. This is slow.
	if (do_check) {
		gettimeofday(&before, NULL);
		for (int i = 0; i < NUM; i++) {
			for (int j = 0; j < NUM; j++) {
				// c was initialized to zero
				TYPE acc = 0;
				for (int k = 0; k < NUM; k++) {
					acc += a[i][k] * b[k][j];
				}
				if (fabs(c[i][j] - acc) > 1e-6) {
					printf("  c[%d][%d] (%f) != %f\n", i, j, c[i][j], acc);
					exit(EXIT_FAILURE);
				}
			}
		}
		gettimeofday(&after, NULL);
		secs = (after.tv_sec - before.tv_sec) + (after.tv_usec - before.tv_usec)/1000000.0;
		printf("The result was correct, and the check took %2.3lf seconds\n", secs);
	}

    // free memory
	free (buf1);
	free (buf2);
	free (buf3);
	free (buf4);
}   

int main()
{
    Initialize();
    
    for (int i = 1; i <= 100; i++) {
        MultiplyOnce(i, false);
    }

    return 0;
}
