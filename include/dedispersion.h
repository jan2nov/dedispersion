void dedispersion_cpu(const unsigned short *input, float *output, const int tsamp, const int channels, const int samples, const int dms, const float *dmshifts, const double step, const unsigned long int nsamp, const float dm_low);

void dedispersion_cpu_band(const unsigned short *input, float *output, const int tsamp, const int channels, const int samples, const int dms, const float *dmshifts, const double step, const unsigned long int nsamp, const float dm_low, unsigned int number_of_bandpass);
