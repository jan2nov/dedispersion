#!/bin/bash

for nsamples in 32 64 96 128 160 192 224 256 512
do
#for idms in 2 3 5 6 10 15 25 30 32 50 75 125 150 250 375 750 
#for idms in 2 3 4 5 6 10 12 15 20 25 30 50 60 75 100 125 150 250 300 375 500 
for idms in 2 4 5 8 10 16 20 25 32 40 50 80 100 160 200 400 800 
do
for ichan in 2 4 8 16 32 64 128 256 512 #1024
do
	echo $nsamples " " $ichan " " $idms
	echo "// dedispersion settings" > ./include/params.h
	echo "#define NSAMPLES  $nsamples" >>  ./include/params.h
	echo "#define RUNS 7" >>  ./include/params.h
	echo "#define DIVINDM $idms" >>  ./include/params.h
	echo "#define DIVINCHAN $ichan" >>  ./include/params.h
	echo "//transpose settings" >>  ./include/params.h
	echo "#define TR_BLOCK 32" >>  ./include/params.h
	echo "//fake signal settings" >>  ./include/params.h
	echo "#define SIGNAL_TIME 5" >>  ./include/params.h
	make -j -B
	sleep 5 
	numactl -C 0-63 ./dedispersion-cpu 1550 2048 300 0.000064 90.0 800 0.25 300.0
done
done
done
