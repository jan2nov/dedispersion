Dedispersion - cpu
==================
Dedispersion kernel using a fake signal generator as an input data. Now is implemented only the inverse gaussian type of fake signal.

For now its able to run only one DM plan. The length of the signal is currently set to at least 5 seconds. To change that edit the file: "include/params.h" argument SIGNAL_TIME. 

The performance changes with changing defined divisions in time_samples, channels, and nsamples (blocking). To find the best one for concrete cpu and signal definitions needs to be autotuned. All these parameters are set in file: "include/params.h".

Now using cilkplus intel array notation extension. 
Req: gcc => 4.9.2 (for cilkplus).
     Now default intel ICC compiler is set (because the gcc can't very well work with cilkplus vectorization).
     GSL library for adding noise

How to install
==============
It should work the basic way.  
  > cmake .    
  > make    
  > ----> creates "dedispersion-cpu"

How to launch
=============
The executable "dedispersion-cpu" needs 8 arguments:

  > example to launch:  
  >  ./dedispersion-cpu 1550 128 300 0.000064 90.0 150 1 0.0 1
    
./dedispersion-cpu [high_frequency] [channels] [total_bandwidth] [time_sampling(in seconds)] [signal_dm_position] [#dm] [dm_step dm_start] [number_of_bandpass]
