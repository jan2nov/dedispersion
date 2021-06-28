#include "params.h"
#include <cilk/cilk.h>
#include <math.h>

void dedispersion_cpu(const unsigned short *input, float *output, const int tsamp,
                    const int channels, const int samples, const int dms, const float *dmshifts,
                    const double step, const unsigned long int nsamp, const float dm_low)
{
#pragma omp parallel //for schedule(auto)
{
#pragma omp for schedule(auto) collapse(2) 
        for (int i = 0; i < samples; i+=NSAMPLES){
                for (int kk=0; kk < dms; kk+=DIVINDM){
                        for (int jj = 0; jj < channels; jj+=DIVINCHAN){
                        for (int k = kk; k < kk + DIVINDM; k++){
                                float shift_temp = dm_low*tsamp+(k)*(step*tsamp);  //must be tsamp of radio not samples
                                register int local[NSAMPLES];
//				  int local[NSAMPLES];                                
                                if (jj == 0){
                                        local[:] = 0;
                                } else
                                {
                                        local[:] = output[(k)*samples+i:NSAMPLES];
                                }
                                for (int j = jj; j < jj + DIVINCHAN; j++){
                                        register int shift  = floor(dmshifts[j]*shift_temp);
                                        local[:] += input[j*nsamp + i + shift:NSAMPLES];
                                }
                                output[(k)*samples+i:NSAMPLES] = (float)local[:];
                        }//k
                        } // jj
                } //kk
        } //samples i
}
        output[0:dms*samples] /= channels;
}


void dedispersion_cpu_band(const unsigned short *input, float *output, const int tsamp,
                    const int channels, const int samples, const int dms, const float *dmshifts,
                    const double step, const unsigned long int nsamp, const float dm_low,
		    unsigned int number_of_bandpass)
{
	unsigned int chan_per_band = channels/number_of_bandpass;
        for (int i = 0; i < samples; i+=NSAMPLES){
                for (int kk=0; kk < dms; kk++){
			for (int band = 0; band < number_of_bandpass; band++){
                        for (int jj = 0; jj < chan_per_band; jj++){
                                float shift_temp = dm_low*tsamp+(kk)*(step*tsamp);  //must be tsamp of radio not samples
                                register int local[NSAMPLES];
//				  int local[NSAMPLES];                                
                                if (jj == 0){
                                        local[:] = 0;
                                } else
                                {
                                        local[:] = output[band*dms*samples + (kk)*samples + i:NSAMPLES];
                                }
                                register int shift  = floor(dmshifts[(jj + band*chan_per_band)]*shift_temp);
                                local[:] += input[(jj + band*chan_per_band)*nsamp + i + shift:NSAMPLES];
                                output[band*dms*samples + (kk)*samples + i:NSAMPLES] = (float)local[:];
                        } // jj
			} //band
                } //kk
        } //samples i
        output[0:dms*samples*number_of_bandpass] /= channels;
}
