#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

void write_results(const int channels, const int ndms, const float fch1, const float total_bandwidth, const float selected_dm, const float time_sampling, const unsigned long int reduced_nsamples, const double time_dedisp, const double time_trans){

	FILE *fp_out;

	 char filename[200];
        sprintf(filename, "time-results.dat");
                if ((fp_out=fopen(filename, "a")) == NULL) {
                        fprintf(stderr, "Error opening results output data file!\n");
                        exit(0);
                }


	fprintf(fp_out, "%d %d %lf %lf %lf %lf %lu %lf %lf %lf\n", channels, ndms, fch1, total_bandwidth, selected_dm, time_sampling, reduced_nsamples, time_trans, time_dedisp, time_sampling*reduced_nsamples/(time_dedisp));

	fclose(fp_out);
}
