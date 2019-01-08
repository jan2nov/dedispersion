#include "params.h"
#include <cilk/cilk.h>
#include <math.h>

void dedispersion_v21(const unsigned short *input, float *output, const int tsamp,
                    const int channels, const int samples, const int dms, const float *dmshifts,
                    const double step, const unsigned long int nsamp, const float dm_low)
{
#pragma omp parallel //for schedule(auto)
{
#pragma omp for schedule(auto) collapse(2) 
        for (int i = 0; i < samples; i+=IB){
                for (int kk=0; kk < dms; kk+=IC){
                        for (int jj = 0; jj < channels; jj+=IJ){
                        for (int k = kk; k < kk + IC; k++){
                                float shift_temp = dm_low*tsamp+(k)*(step*tsamp);  //must be tsamp of radio not samples
                                int local[NSAMPLES];
                                if (jj == 0){
                                        local[:] = 0;
                                } else
                                {
                                        local[:] = output[(k)*samples+i:NSAMPLES];
                                }
                                for (int j = jj; j < jj + IJ; j++){
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
