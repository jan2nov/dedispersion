#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include "fake_signal.h"
#include "shifts.h"
#include "dedispersion.h"
#include "params.h"
#include "transpose.h"
#include "write_raw_data.h"

int main(int argc, char *argv[])
{
	
	double time_start, time_end;
	int maximum_width = 10;
	unsigned short *signal;
	float selected_dm = 90;
	float fch1 = 1550.0;
	float total_bandwidth = 300.0f;
	float time_sampling = 0.000064f; //0.0625f;
	int sampling_rate = 1/time_sampling;
	int channels = 128;
	float chan_ban = -total_bandwidth/channels;
	unsigned long int total_nsamples = 0;

        // Obtain the channel shifts for concrete channel bandwidth ('chan_ban') and high frequency ('fch1') for #number of channels ('channels')
        // note: the channel bandwidth need to be negative (going from high to low frequencies)
        float *shifts;
        shifts = (float*) malloc(channels*sizeof(float));
        get_shifts(shifts, fch1, chan_ban, channels);

	fake_signal(&signal, &total_nsamples, shifts, selected_dm, fch1, channels, total_bandwidth, time_sampling);
	fflush(stdout);

	// searching plan basic setup
	int ndms = 100;
	float dm_step = 1.0;
	float dm_start = 0.0;
	int maxshift = 0;
	int max_high;
	
	max_high = ceil((dm_start + dm_step*ndms)*shifts[channels - 1]*sampling_rate);
	if ( (max_high > maxshift) ) maxshift = max_high;

	//reduced number of samples by the maxshift in dispersion by the plan and performance optimizations
	unsigned long int reduced_nsamples = ( (total_nsamples - maxshift)/NSAMPLES*NSAMPLES );

	// check the ndms to be a multiply of DIVINDM
	ndms = ( (ndms + DIVINDM - 1)/DIVINDM)*DIVINDM;


	float *dedispersed_signal;
	size_t dedispersed_signal_length = ndms*reduced_nsamples;
	dedispersed_signal = (float *) malloc(dedispersed_signal_length*sizeof(float));

	//print basic info of the signal
	printf("\t**********************************************\n");
        printf("\t              Basic Informations             \n");
        printf("\t**********************************************\n");
	printf("\tSamples computed per block:  \t%*d\n"   , maximum_width, NSAMPLES);
	printf("\tChannels computed per block: \t%*d\n"   , maximum_width, DIVINCHAN);
	printf("\tDMs computed per block:      \t%*d\n"   , maximum_width, DIVINDM);
	printf("\tMaxshift for the plan:       \t%*d\n"   , maximum_width, maxshift);
	printf("\tComputing # of dm trials     \t%*d\n"   , maximum_width, ndms);
	printf("\tTotal number of samples:     \t%*lu\n"  , maximum_width, total_nsamples);
	printf("\tReduced number of samples:   \t%*lu\n"  , maximum_width, reduced_nsamples);
        printf("\tSize of the input signal:    \t%*f GB\n", maximum_width, (float)(channels*total_nsamples*sizeof(unsigned short)/1024.0/1024.0/1024.0));
        printf("\tSize of the output:          \t%*f GB\n", maximum_width, (float)(dedispersed_signal_length*sizeof(float)/1024.0/1024.0/1024.0));
        printf("\t**********************************************\n");
	fflush(stdout);

	// tranpose the signal
	unsigned short *transposed_signal;
	transpose(&transposed_signal, signal, channels, total_nsamples);	
	fflush(stdout);

//        for (int i = 0; i < (int)total_nsamples; i++){
//                for (int j = 0; j < channels; j++){
//                        printf("%hu ",signal[i*channels + j]);
//                }
//                printf("\n");
//        }
//
//	printf("\n");
//        for (int i = 0; i < (int)channels; i++){
//                for (int j = 0; j < (int)total_nsamples; j++){
//                        printf("%hu ",transposed_signal[i*total_nsamples + j]);
//                }
//                printf("\n");
//        }

	printf("\n\tLaunching dedispersion for range: %d ...\n", 0);
	time_start = omp_get_wtime();
		dedispersion_cpu(transposed_signal, dedispersed_signal, sampling_rate, channels, reduced_nsamples, ndms, shifts, dm_step, total_nsamples, dm_start);
	time_end = omp_get_wtime() - time_start;
	printf("\t\tdone in: %lf s. Fraction of real-time: %lf\n\n", time_end, reduced_nsamples/(sampling_rate*time_end));
	fflush(stdout);

	write_raw_data(dedispersed_signal, ndms, reduced_nsamples);
	fflush(stdout);

        printf("\n\t**********************************\n");
        printf( "\t        #That's all folks!        \n");
        printf( "\t**********************************\n");

	free(shifts);
	free(signal);
	free(transposed_signal);
	free(dedispersed_signal);

	return 0;
}
