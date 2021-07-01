#include <math.h>
#include <omp.h>
#include <string.h>

//void textbook_msd_parallel(unsigned int first, unsigned int second, unsigned short* in_data, double* mean, double* sd, bool outlier_rejection, double old_mean, double old_stdev, double sigma){
void msd_parallel_basic(size_t samples, float* in_data, double* mean, double* sd){

	double total = 0.0;
	#pragma omp parallel for reduction(+:total)
	for (size_t c = 0; c < samples;c++)
                total += in_data[c];

	double temp_mean = total/samples;

	total = 0.0;
	#pragma omp parallel for reduction(+:total)
        for(size_t j = 0; j < samples; j++)
                total += (double)((in_data[j] - temp_mean)*(in_data[j] - temp_mean));

	*mean = temp_mean;
	*sd = (double)sqrt(total / (double)(samples));
}

void msd_parallel_basic_double(size_t samples, double* in_data, double* mean, double* sd){

	double total = 0.0;
	#pragma omp parallel for reduction(+:total)
	for (size_t c = 0; c < samples;c++)
                total += in_data[c];

	double temp_mean = total/samples;

	total = 0.0;
	#pragma omp parallel for reduction(+:total)
        for(size_t j = 0; j < samples; j++)
                total += (in_data[j] - temp_mean)*(in_data[j] - temp_mean);

	*mean = temp_mean;
	*sd = (double)sqrt(total / (double)(samples));
}


void msd_parallel_char(size_t samples, unsigned short* in_data, double* mean, double* sd){

	double total = 0.0;
	#pragma omp parallel for reduction(+:total)
	for (size_t c = 0; c < samples;c++)
                total += in_data[c];

	double temp_mean = total/samples;

	total = 0.0;
	#pragma omp parallel for reduction(+:total)
        for(size_t j = 0; j < samples; j++)
                total += (double)((in_data[j] - temp_mean)*(in_data[j] - temp_mean));

	*mean = temp_mean;
	*sd = (double)sqrt(total / (double)(samples));
}
