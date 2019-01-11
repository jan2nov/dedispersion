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

	if( argc < 9){
		printf("ERROR: Need more arguments, please see the README.md\n\n");
		exit(100);
	}

	
	double time_start, time_end;
	int maximum_width = 10;
	unsigned short *signal;
	float selected_dm = atof(argv[5]);
	float fch1 = atof(argv[1]);
	int channels = atoi(argv[2]);
		if (channels < DIVINCHAN) {
			printf("ERROR: The DIVINDM is bigger than asked # channels\n\n");
			exit(100);
		}
	float total_bandwidth = atof(argv[3]);
	float time_sampling = atof(argv[4]);
	int sampling_rate = 1/time_sampling;
	float chan_ban = -total_bandwidth/channels;
	unsigned long int total_nsamples = 0;

	
        // Obtain the channel shifts for concrete channel bandwidth ('chan_ban') and high frequency ('fch1') for #number of channels ('channels')
        // note: the channel bandwidth need to be negative (going from high to low frequencies)
        float *shifts;
        shifts = (float*) malloc(channels*sizeof(float));
        get_shifts(shifts, fch1, chan_ban, channels);

	fake_signal(&signal, &total_nsamples, shifts, selected_dm, fch1, channels, total_bandwidth, time_sampling);
	fflush(stdout);

	//write signal to disk
//	signal_raw_data(signal, total_nsamples, channels);

	// searching plan basic setup
	int ndms = atoi(argv[6]);;
	float dm_step = atof(argv[7]);
	float dm_start = atof(argv[8]);
	int maxshift = 0;
	int max_high;
	
	max_high = ceil((dm_start + dm_step*ndms)*shifts[channels - 1]*sampling_rate);
	if ( (max_high > maxshift) ) maxshift = max_high;

	//reduced number of samples by the maxshift in dispersion by the plan and performance optimizations
	//the NSAMPLES need to be less then total_nsamples - maxshift
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
	printf("\tDM step is set to            \t%*lf\n"   , maximum_width, dm_step);
	printf("\tStart search from dm:        \t%*lf\n"   , maximum_width, dm_start);
	printf("\tTotal number of samples:     \t%*lu\n"  , maximum_width, total_nsamples);
	printf("\tReduced number of samples:   \t%*lu\n"  , maximum_width, reduced_nsamples);
        printf("\tSize of the input signal:    \t%*f GB\n", maximum_width, (float)(channels*total_nsamples*sizeof(unsigned short)/1024.0/1024.0/1024.0));
        printf("\tSize of the output:          \t%*f GB\n", maximum_width, (float)(dedispersed_signal_length*sizeof(float)/1024.0/1024.0/1024.0));
        printf("\t**********************************************\n");
	fflush(stdout);

	// tranpose the signal
	unsigned short *transposed_signal;
	
	double time_trans = 0;
	transpose(&transposed_signal, &time_trans, signal, channels, total_nsamples);	
	fflush(stdout);

	//write transposed signal to disk; just for debugging
//	signal_raw_data(signal, total_nsamples, channels);

	// Launching de-dispersion kernel
	double time_dedisp = 0;
	printf("\tLaunching %dx dedispersion for range: %d ...\n", RUNS,0);
	for (int i = 0; i < RUNS; i++){
		time_start = omp_get_wtime();
			dedispersion_cpu(transposed_signal, dedispersed_signal, sampling_rate, channels, reduced_nsamples, ndms, shifts, dm_step, total_nsamples, dm_start);
		time_end = omp_get_wtime() - time_start;
		time_dedisp += time_end;
		printf("\t\t\t#%d: %lf seconds\n", i, time_end);
	}
	time_dedisp = time_dedisp/RUNS;
	printf("\t\tdone in average: %lf s. Fraction of real-time: %lf\n\n", time_dedisp, reduced_nsamples/(sampling_rate*time_dedisp));
	fflush(stdout);

	//uncomment to raw write data
//	write_raw_data(dedispersed_signal, ndms, reduced_nsamples, dm_step, dm_start);
	fflush(stdout);

	// end of the code
        printf("\n\t**********************************\n");
        printf( "\t        #That's all folks!        \n");
        printf( "\t**********************************\n");

	//clean-up
	free(shifts);
	free(signal);
	free(transposed_signal);
	free(dedispersed_signal);

	return 0;
}
