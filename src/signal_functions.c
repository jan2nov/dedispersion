#include <math.h>

#define M_PI 3.14159265358979323846


// the function returns normalized scale to be multiplied later to create fake data with sampling number of 'length' and the position of the maximum point
void inverse_gaussian(float *output, int length, float sigma, int *max_pos){
	// width of the gaussian -- can be changed is just an estimate
	// the setup wid seems reasonable to get the tail of the innverse gaussian
        float wid = 2*sigma*pow(3*log(30),0.5);
        float step = wid*sigma/(length);
        float maximum =0.0f;
        for(int i = 0; i < length; i++){
                float x = (i+1)*step;
                output[i] = pow(1.0*sigma/(2*M_PI*pow(x,3)),0.5)*exp((-sigma*pow(x - 1.0,2))/(2*pow(1.0,2)*x));
                if (maximum <= output[i]) {
                        maximum = output[i];
                        *max_pos = i;
                }
        }
        // normalization part
        for (int i = 0; i < length; i++){
	        output[i] = output[i]/maximum;
        }
}

void signal_at_dm_shift(int *shift_index, const float *shifts, const int channels, const float dm, const unsigned int sampling_rate){
	for (int i = 0; i < channels; i++)
		shift_index[i] = floor(shifts[i]*(dm*sampling_rate));
}
