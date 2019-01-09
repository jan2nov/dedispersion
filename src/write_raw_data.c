#include <stdio.h>
#include <omp.h>
#include <stdlib.h>


void write_raw_data(float *dedispersed_signal, const int ndms, const int reduced_nsamples){

	double time_start, time_end;

        FILE *fp_out;
        char filename[200];
        sprintf(filename, "analysed-dm_%i-%i.dat", ndms, 0);
                if ((fp_out=fopen(filename, "wb")) == NULL) {
                        fprintf(stderr, "Error opening output file!\n");
                        exit(0);
                }


	printf("\tWriting the raw data to file '%s' ...\n", filename);	
	time_start = omp_get_wtime();
        for (int dm_count = 0; dm_count < ndms; dm_count++) {
                for(int j = 0; j < reduced_nsamples; j++) {
                        fprintf(fp_out, "%d %d %.8lf\n", j, dm_count, dedispersed_signal[dm_count*reduced_nsamples + j]);
                }
        }
	time_end = omp_get_wtime()-time_start;
	printf("\t\tdone in %lf seconds.\n\n",time_end);

        fclose(fp_out);
}
