#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>
#include "msd.h"


void write_raw_data(float *dedispersed_signal, const int ndms, const unsigned long int reduced_nsamples, const float dm_step, const float dm_start, unsigned int band){

	double time_start, time_end;

        FILE *fp_out;
        char filename[200];
        sprintf(filename, "analysed-dm_%i-%i.dat", ndms, 0);
                if ((fp_out=fopen(filename, "wb")) == NULL) {
                        fprintf(stderr, "Error opening output file to write ddtr results!\n");
                        exit(0);
                }


	printf("\tWriting the raw data to file '%s' ...\n", filename);	
	time_start = omp_get_wtime();
	for (unsigned int b = 0; b < band; b++){
	        for (int dm_count = 0; dm_count < ndms; dm_count++) {
	                for(int j = 0; j < reduced_nsamples; j++) {
	                        fprintf(fp_out, "%d %d %lf %.8lf\n", b, j, (dm_count*dm_step + dm_start), dedispersed_signal[b*ndms*reduced_nsamples + dm_count*reduced_nsamples + j]);
	                }
	        }
	}
	time_end = omp_get_wtime()-time_start;
	printf("\t\tdone in %lf seconds.\n\n",time_end);

        fclose(fp_out);
}


void write_spd(float *dedispersed_signal, const int ndms, const unsigned long int nsamples, const float dm_step, const float dm_start, unsigned int band){


	double mean = 0.0;
	size_t samples = ndms*nsamples*band;
	double stddev = 0.0;
	
	msd_parallel_basic(samples, dedispersed_signal, &mean, &stddev);
	printf("\tDedispersed signal statistics ---> mean: %lf, std: %lf\n", mean, stddev);

	FILE *fp_out;
        char filename[200];
        sprintf(filename, "SPD-dm_%i-%i.dat", ndms, 0);
                if ((fp_out=fopen(filename, "wb")) == NULL) {
                        fprintf(stderr, "Error opening output file to write ddtr results!\n");
                        exit(0);
                }

	double temp = 0.0;
	for (unsigned int b = 0; b < band; b++){
       	        for (int dm_count = 0; dm_count < ndms; dm_count++) {
       	                for(int j = 0; j < nsamples; j++) {
				temp = (dedispersed_signal[b*ndms*nsamples + dm_count*nsamples + j] - mean)/stddev;
       	                        fprintf(fp_out, "%d %d %lf %.8lf %.8lf\n", b, j, (dm_count*dm_step + dm_start), dedispersed_signal[b*ndms*nsamples + dm_count*nsamples + j], temp);
       	                }
	        }
       	}
}

void signal_raw_data(unsigned short *signal, const unsigned long int nsamples, const int channels){

	double time_start, time_end;
	
	FILE *fp_out;

	char filename[200];
        sprintf(filename, "fake_signal.dat");
                if ((fp_out=fopen(filename, "wb")) == NULL) {
                        fprintf(stderr, "Error opening output file for fake signal!\n");
                        exit(0);
		}

	printf("\tWriting the fake signal to file '%s' ...\n", filename);
        time_start = omp_get_wtime();
	for (int i = 0; i < channels; i++){
        	for (int j = 0; j < nsamples; j++){
			fprintf(fp_out,"%d %d %3d\n", i, j, signal[j*channels + i]);
                }
        }
	time_end = omp_get_wtime()-time_start;
        printf("\t\tdone in %lf seconds.\n\n",time_end);
	
	fclose(fp_out);
}


void signal_raw_noise(double *signal, const unsigned long int nsamples, const int channels){

	double time_start, time_end;
	
	FILE *fp_out;

	char filename[200];
        sprintf(filename, "noise_data.dat");
                if ((fp_out=fopen(filename, "wb")) == NULL) {
                        fprintf(stderr, "Error opening output file for fake signal!\n");
                        exit(0);
		}

	printf("\tWriting the noise to file '%s' ...\n", filename);
        time_start = omp_get_wtime();
	for (int i = 0; i < channels; i++){
        	for (int j = 0; j < nsamples; j++){
			fprintf(fp_out,"%d %d %lf\n", i, j, signal[i*nsamples + j]);
                }
        }
	time_end = omp_get_wtime()-time_start;
        printf("\t\tdone in %lf seconds.\n\n",time_end);
	
	fclose(fp_out);
}


