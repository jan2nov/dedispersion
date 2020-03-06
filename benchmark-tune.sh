#!/bin/bash

for nsamples in 32
do
for idms in 2 3
do
for ichan in 2 4 8 16 32 64
do
	echo $nsamples " " $ichan " " $idms
	echo "// dedispersion settings" > ./include/params.h
	echo "#define NSAMPLES  $nsamples" >>  ./include/params.h
	echo "#define RUNS 5" >>  ./include/params.h
	echo "#define DIVINDM 2" >>  ./include/params.h
	echo "#define DIVINCHAN 2" >>  ./include/params.h
	echo "//transpose settings" >>  ./include/params.h
	echo "#define TR_BLOCK 32" >>  ./include/params.h
	echo "//fake signal settings" >>  ./include/params.h
	echo "#define SIGNAL_TIME 5" >>  ./include/params.h
	make -B
	./dedispersion-cpu 1550 4096 300 0.000064 90.0 1500 1 0.0
done
done
done
