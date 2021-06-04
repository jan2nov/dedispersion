#!/bin/bash


for i in {1..80} 
do
#	KMP_AFFINITY=scatter OMP_NUM_THREADS=$i ./dedispersion-cpu 1550 4096 300 0.000064 90.0 750 0.2 150.0 | grep  "done in average" | awk '{print $(i) " " $4}'
#	KMP_AFFINITY=scatter OMP_NUM_THREADS=$i ./dedispersion-cpu 1468 336 336 0.001265 350.0 2048 0.96 0.0 | grep  "done in average" | awk '{print $(i) " " $4}'
	KMP_AFFINITY=scatter OMP_NUM_THREADS=$i ./dedispersion-cpu 800 16384 400 0.00098304 90.0 4000 0.05 0.0 | grep  "done in average" | awk '{print $(i) " " $4}'
done
#OMP_NUM_THREADS=8 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 8 " " $4}'
#OMP_NUM_THREADS=16 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 16 " " $4}'
#OMP_NUM_THREADS=32 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 32 " " $4}'
#OMP_NUM_THREADS=64 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 64 " " $4}'
#OMP_NUM_THREADS=96 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 96 " " $4}'
#OMP_NUM_THREADS=128 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 128 " " $4}'
