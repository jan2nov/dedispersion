#include <malloc.h>
#include <string.h>
#include <omp.h>
#include "params.h"
#include "shifts.h"
#include "signal_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


#define MAX 255

void fake_signal(unsigned short **signal, unsigned long int *nsamples, const float *shifts, const float selected_dm, const float fch1, const int channels, const float total_bandwidth, const float time_sampling)
{
	int maximum_width = 10;
	//time measurement
        double time_start, time_end;

	// setup of the fake signal
//	int channels = 16;
//	float fch1 = 1550;
//	float total_bandwidth = 300;
	float chan_ban = -total_bandwidth/channels;
//	float time_sampling = 0.000064f;
        unsigned int sampling_rate = (unsigned int)(1/time_sampling);
	int signal_length_time = 3;
	// position of the signal in #samples
	int signal_time_position = 1.5*sampling_rate;
	// read from the launch at what dispersion measure to put the fake signal
//	float selected_dm = atof(argv[1]); 

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
        printf("\t**********************************\n\n");

	// Getting the position of the max peak for the selected type of signal and the normalized scale factor
	float *func_scale;
	int func_length = 100; //note: the lenght must be less then sampling rate
	int max_pos = 0;
	func_scale = (float *)malloc(func_length*sizeof(float));
	inverse_gaussian(func_scale,func_length, 0.5, &max_pos);

	int *shifts_index;
	shifts_index = (int *) malloc(channels*sizeof(int));
	signal_at_dm_shift(shifts_index,shifts, channels,selected_dm, sampling_rate);

	// fill the signal with the data
//	unsigned short *signal;
	unsigned int signal_length;
	// check if the signal start + shift is still in the signal_length
	if ( signal_length_time*sampling_rate > (unsigned int)( signal_time_position + shifts_index[channels-1])){
                signal_length = signal_length_time*sampling_rate;
        } else {
		printf("\tNote: Signal needs to be at least of length: %d samples. Increasing...\n", signal_time_position + shifts_index[channels-1]);
                signal_length = signal_length_time*sampling_rate + signal_time_position + shifts_index[channels-1];
        }
	signal_length = signal_length/TR_BLOCK*TR_BLOCK; // the transpose kernel needs signal to be multiple of the tr_block
	size_t signal_size = signal_length*channels;
	*nsamples = signal_length;
	*signal = (unsigned short *) malloc(signal_size*sizeof(unsigned short));
	
	printf("\tGenerating fake signal with %d samples (%lf s) ...", signal_length,(float)signal_length/sampling_rate);
	time_start = omp_get_wtime();
	memset(*signal, 0, signal_size*sizeof(unsigned short));
	for (int i = 0; i < func_length; i++){
		int time_pos = (i - max_pos + signal_time_position)*channels; // inverse gauss case
		for(int j = 0; j < channels; j++){
			(*signal)[j + channels*shifts_index[j] + time_pos] = func_scale[i]*MAX;
		}	
	}
	time_end = omp_get_wtime() - time_start;
	printf("\n\t\tdone in %lf seconds.\n\n",time_end);

	//clean-up
//	free(shifts);
	free(func_scale);
	free(shifts_index);
}
