void fake_signal(double **signal, unsigned long int *nsamples, double *pwrSignal, const float *shifts, const float selected_dm, const float fch1, const int channels, const float total_bandwidth, const float time_sampling);

void noise_background(double **noise_signal, double *pwrNoise, unsigned long int nsamples, const int nchans);

void add_noise(double **signal, double *noise_signal, double pwrSignal, double pwrNoise, float DESIRED_SNR, unsigned long int total_nsamples, const int channels);
void rescale_signal(unsigned short **rescale_signal, double *signal, unsigned long int nsamples, const int channels);
