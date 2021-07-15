#include <math.h>
#include <omp.h>
#include <string.h>

void SumBands(float *destination, float *source, size_t nElements){
	#pragma omp parallel for
	for(size_t s=0; s<nElements; s++){
		destination[s] = destination[s] + source[s];
	}
}

void NormalizeBand(float *dedispersed_data, int *nSums, double *mean, double *stdev, size_t nsamples, size_t ndms){
	for(size_t d=0; d<ndms; d++){
		for(size_t s=0; s<nsamples; s++){
			size_t pos = d*nsamples + s;
			int sums = nSums[pos];
			dedispersed_data[pos] = (dedispersed_data[pos] - mean[sums])/stdev[sums];
		}
	}
}