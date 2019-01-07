#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <omp.h>
#include "fake_signal.h"

int main(int argc, char *argv[])
{

	unsigned short *signal;
	float selected_dm = 90;
	float fch1 = 1550.0;
	float total_bandwidth = 300;
	float time_sampling = 0.000064f;
	int channels = 16;

	fake_signal(&signal, selected_dm, fch1, channels, total_bandwidth, time_sampling);

	for(int i = 0; i < channels; i++)
		for(int j = 0; j < 46875; j++)
			if (signal[j*channels + i] != 0) printf("%d\n", signal[j*channels + i]);

        printf("\n\t**********************************\n");
        printf("\t        #That's all folks!        \n");
        printf("\t**********************************\n");

	free(signal);

	return 0;
}
