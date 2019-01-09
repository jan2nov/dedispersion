#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#include "params.h"

void transpose_scalar_block(const unsigned short *A, unsigned short *B, const unsigned long lda, const unsigned long ldb) {
    #pragma omp parallel for
    for(int k=0; k < TR_BLOCK; k++) {
        for(int l=0; l< TR_BLOCK; l++) {
            ( B)[(unsigned long)(l*ldb + k)] = A[(unsigned long)(k*lda + l)];
        }
    }
}

void transpose_block(const unsigned short *A, unsigned short **B, const unsigned long n, const unsigned long m, const unsigned long lda, const unsigned long ldb) {
    #pragma omp parallel for collapse(2) schedule(guided)
    for(unsigned long i=0; i < n; i+=TR_BLOCK) {
        for(unsigned long j=0; j < m; j+=TR_BLOCK) {
            transpose_scalar_block(&A[(unsigned long)(i*lda +j)], &(* B)[(unsigned long)(j*ldb + i)], lda, ldb);
        }
    }
}

void transpose(unsigned short **output, unsigned short *input, const int channels, const unsigned long int nsamples){
	
	double time_start, time_end;

	size_t output_size = channels*nsamples;
	*output = (unsigned short *) malloc(output_size*sizeof(unsigned short));

	unsigned long rows_round = floor((float)channels/TR_BLOCK)*TR_BLOCK;
        unsigned long columns_round = floor((float)nsamples/TR_BLOCK)*TR_BLOCK;

	printf("\n\tTransposition for a block size: %d ...\n", TR_BLOCK);
	time_start = omp_get_wtime();
                transpose_block(input,output,nsamples,channels,rows_round,columns_round);
        time_end = omp_get_wtime() - time_start;
	float thr = 2*output_size*sizeof(unsigned short)/1024.0/1024.0/1024.0/time_end;
	printf("\t\tdone in: %lf. Bandwidth: %lf GB/s.\n", time_end, thr);

}
