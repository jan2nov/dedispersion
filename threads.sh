#!/bin/bash


for i in {1..128} 
do
	KMP_AFFINITY=scatter OMP_NUM_THREADS=$i ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print $(i) " " $4}'
done
#OMP_NUM_THREADS=8 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 8 " " $4}'
#OMP_NUM_THREADS=16 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 16 " " $4}'
#OMP_NUM_THREADS=32 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 32 " " $4}'
#OMP_NUM_THREADS=64 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 64 " " $4}'
#OMP_NUM_THREADS=96 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 96 " " $4}'
#OMP_NUM_THREADS=128 ./dedispersion-cpu-old 1550 4096 300 0.000064 90.0 1500 0.1 0.0 | grep  "done in average" | awk '{print 128 " " $4}'
