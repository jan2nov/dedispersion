#include <math.h>

void get_shifts(float *shifts, const float fch1, const float foff, const int channels){
	for(int i = 0; i < channels; i++){
		shifts[i] = (float) ( 4148.741601f * ( ( 1.0 / pow((double) ( fch1 + ( foff * i ) ), 2.0f) ) - ( 1.0 / pow((double) fch1, 2.0f) ) ) );
	}
}
                                      
