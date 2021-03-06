#include <stdio.h>
#include <omp.h>
#include <stdlib.h>


void write_raw_data(float *dedispersed_signal, const int ndms, const unsigned long int reduced_nsamples, const float dm_step, const float dm_start){

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
        for (int dm_count = 0; dm_count < ndms; dm_count++) {
                for(int j = 0; j < reduced_nsamples; j++) {
                        fprintf(fp_out, "%d %lf %.8lf\n", j, (dm_count*dm_step + dm_start), dedispersed_signal[dm_count*reduced_nsamples + j]);
                }
        }
	time_end = omp_get_wtime()-time_start;
	printf("\t\tdone in %lf seconds.\n\n",time_end);

        fclose(fp_out);
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
