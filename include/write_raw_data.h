void write_raw_data(float *dedispersed_signal, const int ndms, const unsigned long int reduced_nsamples, const float dm_step, const float dm_start, unsigned int band);
void signal_raw_data(unsigned short *signal, const unsigned long int nsamples, const int channels);
void signal_raw_noise(double *signal, const unsigned long int nsamples, const int channels);
void write_spd(float *dedispersed_signal, const int ndms, const unsigned long int nsamples, const float dm_step, const float dm_start, unsigned int band);
