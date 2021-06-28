#include <malloc.h>
#include <string.h>
#include <omp.h>
#include "params.h"
#include "shifts.h"
#include "signal_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>


#define MAX 255

void fake_signal(double **signal, unsigned long int *nsamples, double *pwrSignal, const float *shifts, const float selected_dm, const float fch1, const int channels, const float total_bandwidth, const float time_sampling)
{
	int maximum_width = 10;
	//time measurement
        double time_start, time_end;

	// setup of the fake signal
	float chan_ban = -total_bandwidth/channels;
        unsigned int sampling_rate = (unsigned int)(1/time_sampling);
	int signal_length_time = SIGNAL_TIME;
	// position of the signal in #samples
	int signal_time_position = 1.5*sampling_rate;
	// read from the launch at what dispersion measure to put the fake signal

	//print info to stdout
        printf("\t**********************************\n");
        printf("\t    Setup for the fake signal     \n");
        printf("\t**********************************\n");
        printf("\tNumber of channels: \t%*d\n",maximum_width,channels);
        printf("\tTotal bandwidth: \t%*.1lf\n",maximum_width,total_bandwidth);
        printf("\tFrequency high: \t%*.1lf\n", maximum_width,fch1);
        printf("\tChannel bandwidth: \t%*.6lf\n",maximum_width,chan_ban);
        printf("\tTime sampling:  \t%*.6lf\n",maximum_width,time_sampling);
        printf("\tSampling rate:  \t%*d\n",maximum_width,sampling_rate);
        printf("\tSignal at DMs:  \t%*f\n",maximum_width,selected_dm);
        printf("\tSignal start:   \t%*f\n",maximum_width,(float)(1.0*signal_time_position/sampling_rate));
        printf("\tSignal length:  \t%*d\n",maximum_width,signal_length_time);

	// Getting the position of the max peak for the selected type of signal and the normalized scale factor
	float *func_scale;
	int func_length = sampling_rate/100.0 + 1; //note: the lenght must be less then sampling rate; now set to at least 1% of the sampling rate
	printf("\tSignal width:   \t%*d\n",maximum_width, func_length);
        printf("\t**********************************\n\n");
	int max_pos = 0;
	func_scale = (float *)malloc(func_length*sizeof(float));
	inverse_gaussian(func_scale,func_length, 0.5, &max_pos);

	int *shifts_index;
	shifts_index = (int *) malloc(channels*sizeof(int));
	signal_at_dm_shift(shifts_index,shifts, channels,selected_dm, sampling_rate);

	// fill the signal with the data
	unsigned int signal_length;
	// check if the signal start + shift is still in the signal_length
	if ( signal_length_time*sampling_rate > (unsigned int)( signal_time_position + shifts_index[channels - 1])){
                signal_length = signal_length_time*sampling_rate;
        } else {
		printf("\tNote: Signal needs to be at least of length: %d samples. Increasing...\n", signal_time_position + shifts_index[channels - 1]);
                signal_length = signal_length_time*sampling_rate + signal_time_position + shifts_index[channels - 1];
        }
	signal_length = (signal_length + TR_BLOCK )/TR_BLOCK*TR_BLOCK; // the transpose kernel needs signal to be multiple of the tr_block
	size_t signal_size = (size_t)signal_length*(size_t)channels;
	*nsamples = signal_length;
	*signal = (double *) malloc(signal_size*sizeof(double));
	
	printf("\tGenerating fake signal with %d samples (%lf s) ...", signal_length,(float)signal_length/sampling_rate);
	time_start = omp_get_wtime();
	memset(*signal, 0, signal_size*sizeof(double));
	double sum_total = 0.0;
	for (int i = 0; i < func_length; i++){
		int time_pos = (i - max_pos + signal_time_position)*channels; // inverse gauss case
		for(int j = 0; j < channels; j++){
			(*signal)[j + channels*shifts_index[j] + time_pos] = func_scale[i]; //round(func_scale[i]*MAX);
			sum_total += func_scale[i]*func_scale[i]; //(round(func_scale[i]*MAX));
		}	
	}
	time_end = omp_get_wtime() - time_start;
	printf("\n\t\tdone in %lf seconds.\n\n",time_end);

	*pwrSignal = (sum_total);//signal_size; 

	//clean-up
	free(func_scale);
	free(shifts_index);
}

void noise_background(double **noise_signal, double *pwrNoise, unsigned long int nsamples, const int nchans){
	double time_start, time_end;
	size_t noise_size = (size_t)(nsamples)*(size_t)(nchans);
	*noise_signal = (double *) malloc(noise_size*sizeof(double));

	//----------------> GSL stuff 
	const gsl_rng_type *rndType;
	gsl_rng *rnd_handle;
	gsl_rng_env_setup();
	//long int seed=(long int) time(NULL);
	long int seed=(long int) 1;
	rndType = gsl_rng_default;
	rnd_handle = gsl_rng_alloc(rndType);
	gsl_rng_set(rnd_handle,seed);
	//----------------> GSL stuff 
	
	double sum_total = 0.0;
	double temp; 
	printf("\tGenerating noise ...");
//////////////
	time_start = omp_get_wtime();
	for (size_t i = 0; i < noise_size; i++){
		temp = gsl_ran_gaussian(rnd_handle, 3.0);
		(*noise_signal)[i] = temp;
		sum_total+= temp*temp;
	}
	time_end = omp_get_wtime() - time_start;
//////////////
	printf("\n\t\tdone in %lf seconds.\n",time_end);

	//noise power
	*pwrNoise = 0.0;
	*pwrNoise = (sum_total);///noise_size; 
	printf("sum_total: %lf noise_size: %zu", sum_total, noise_size);

	gsl_rng_free(rnd_handle);
}


void add_noise(double **signal, double *noise_signal, double pwrSignal, double pwrNoise, float DESIRED_SNR, unsigned long int total_nsamples, const int channels){
	printf("\tAdding noise to the signal");

	double alpha = sqrt(pwrSignal/(DESIRED_SNR*pwrNoise));
	printf("\talpha: %lf\n", alpha);
	for(int i = 0; i < total_nsamples; i++){
		for(int j = 0; j < channels; j++){
			(*signal)[i*channels + j] += alpha*noise_signal[i*channels + j];
		}
	}
	printf("\n\t\tdone.\n");
}

void rescale_signal(unsigned short **rescaled_signal, double *signal, unsigned long int nsamples, const int channels){
	double maximum = -1.0;
	double minimum = signal[0];

	size_t signal_size = (size_t)nsamples*(size_t)channels;
	*rescaled_signal = (unsigned short *) malloc(signal_size*sizeof(unsigned short));
	memset(*rescaled_signal, 0, signal_size*sizeof(unsigned short));

	printf("\tRescale the signal\n");
	for (int i = 0; i < channels; i++){
		for (int j = 0; j < nsamples; j++){
			double temp = signal[j*channels + i];
			if (maximum < temp){
				maximum = temp;
			}
			if (minimum > temp){
				minimum = temp;
			}
		}
	}
	double diff = maximum - minimum;
	printf("\t\tOriginal Min: %lf Original Max: %lf Diff %lf\n", minimum, maximum, diff);
	//min-max rescaling to obtain (0,MAX)
	for (size_t i = 0; i < (channels*nsamples); i++){
		double temp = signal[i];
		(*rescaled_signal)[i] = round(((temp - minimum)/diff)*MAX);
	}
	printf("\t... done.\n");
}
