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
#include "write_results.h"
#include "msd.h"

#include <gsl/gsl_rng.h>

int main(int argc, char *argv[])
{

	if( argc < 10){
		printf("ERROR: Need more arguments, please see the README.md\n\n");
		exit(100);
	}

	
	double time_start, time_end;
	int maximum_width = 10;
	double *signal;
//	unsigned short *signal;
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
	unsigned int number_of_bandpass = atoi(argv[9]);
	if ( (number_of_bandpass < 1) || (number_of_bandpass > channels)) {
		printf("ERROR: The number of bandpass must be bigger than 1 and lower than the number of channels.\n\n");
		exit(101);
	}

	
        // Obtain the channel shifts for concrete channel bandwidth ('chan_ban') and high frequency ('fch1') for #number of channels ('channels')
        // note: the channel bandwidth need to be negative (going from high to low frequencies)
        float *shifts;
	double pwrSignal = 0.0;
        shifts = (float*) malloc(channels*sizeof(float));
        get_shifts(shifts, fch1, chan_ban, channels);

	fake_signal(&signal, &total_nsamples, &pwrSignal, shifts, selected_dm, fch1, channels, total_bandwidth, time_sampling);
//	fake_signal_us(&signal, &total_nsamples, &pwrSignal, shifts, selected_dm, fch1, channels, total_bandwidth, time_sampling);
	fflush(stdout);

	// create noise for adding to the signal
	double *noise_signal;
	double pwrNoise = 0.0;
	if (ADD_NOISE){
		noise_background(&noise_signal, &pwrNoise, total_nsamples, channels);
		add_noise(&signal, noise_signal, pwrSignal, pwrNoise, DEFINED_SNR, total_nsamples, channels);
	}
	fflush(stdout);

	unsigned short *rescaled_signal;
	rescale_signal(&rescaled_signal, signal, total_nsamples, channels);


	//write signal to disk
	signal_raw_data(rescaled_signal, total_nsamples, channels);
	//write noise to disk
	if (ADD_NOISE){
		signal_raw_noise(noise_signal, total_nsamples, channels);
	}

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
	if (total_nsamples < maxshift) {
		printf("ERROR: Signal length too low. Please increase the length.\n");
		printf("Maxshift is: %d samples, signal is lenght of: %lu\n", maxshift, total_nsamples);
		exit(103);
	}

	unsigned long int reduced_nsamples = ( (total_nsamples - maxshift)/NSAMPLES*NSAMPLES );

	// check the ndms to be a multiply of DIVINDM
	ndms = ( (ndms + DIVINDM - 1)/DIVINDM)*DIVINDM;


	float *dedispersed_signal;
	size_t dedispersed_signal_length = ndms*reduced_nsamples*number_of_bandpass;
	dedispersed_signal = (float *) _mm_malloc(dedispersed_signal_length*sizeof(float),64);

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
	printf("\tNumber of dividing channels  \t%*d\n"   , maximum_width, number_of_bandpass);
	if (ADD_NOISE){
	printf("\tPower signal is:             \t%*lf\n", maximum_width, pwrSignal);
	printf("\tPower noise is:              \t%*lf\n", maximum_width, pwrNoise);
	printf("\tDesired signal to noise ratio\t%*.2lf\n", maximum_width, DEFINED_SNR); 
	}
        printf("\t**********************************************\n");
	fflush(stdout);

	// tranpose the signal
	unsigned short *transposed_signal;
	
	double time_trans = 0;
	transpose(&transposed_signal, &time_trans, rescaled_signal, channels, total_nsamples);	
	fflush(stdout);
	//write transposed signal to disk; just for debugging
//	signal_raw_data(transposed_signal, total_nsamples, channels);

	// Launching de-dispersion kernel
	double time_dedisp = 0;
	printf("\tLaunching %dx dedispersion for range: %d ...\n", RUNS,0);
	for (int i = 0; i < RUNS; i++){
		time_start = omp_get_wtime();
//			dedispersion_cpu(transposed_signal, dedispersed_signal, sampling_rate, channels, reduced_nsamples, ndms, shifts, dm_step, total_nsamples, dm_start);
			dedispersion_cpu_band(transposed_signal, dedispersed_signal, sampling_rate, channels, reduced_nsamples, ndms, shifts, dm_step, total_nsamples, dm_start, number_of_bandpass);
		time_end = omp_get_wtime() - time_start;
		time_dedisp += time_end;
		printf("\t\t\t#%d: %lf seconds\n", i, time_end);
	}
	time_dedisp = time_dedisp/RUNS;
	printf("\t\tdone in average: %lf s. Fraction of real-time: %lf\n", time_dedisp, reduced_nsamples/(sampling_rate*time_dedisp));
	printf("\t\tGFLOPS: %lf.\n\n", (reduced_nsamples*channels*ndms)/time_dedisp/1e9);
	fflush(stdout);

	//uncomment to raw write data
	write_raw_data(dedispersed_signal, ndms, reduced_nsamples, dm_step, dm_start, number_of_bandpass);
	fflush(stdout);

	write_spd(dedispersed_signal, ndms, reduced_nsamples, dm_step, dm_start, number_of_bandpass);

	//std 
	
//	double mean = 0.0;
//	double stddev = 0.0;
//	msd_parallel_basic((size_t)(reduced_nsamples*ndms*number_of_bandpass), dedispersed_signal, &mean, &stddev);
//	printf("\tDedispersed signal mean: %lf and stddev: %lf\n", mean, stddev);

	//write results to file
	write_results(channels, ndms, fch1, total_bandwidth, selected_dm, time_sampling, reduced_nsamples, time_dedisp, time_trans);

	// end of the code
        printf("\n\t**********************************\n");
        printf( "\t        #That's all folks!        \n");
        printf( "\t**********************************\n");

	//clean-up
	free(shifts);
	free(signal);
	free(rescaled_signal);
	free(transposed_signal);
	_mm_free(dedispersed_signal);
	if (ADD_NOISE){
		free(noise_signal);
	}

	return 0;
}
