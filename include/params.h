// dedispersion settings
#ifndef NSAMPLES
	#define NSAMPLES  10
#endif
#define RUNS 1
#define DIVINDM 1
#ifndef DIVINCHAN
	#define DIVINCHAN 256
#endif
//transpose settings
#define TR_BLOCK 32
//fake signal settings
#define SIGNAL_TIME 1

// true or false to add noise to the generated signal
#ifndef ADD_NOISE
	#define ADD_NOISE 1
#endif

#ifndef DEFINED_SNR
	#define DEFINED_SNR 3.0
#endif
