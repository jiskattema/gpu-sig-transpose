A full run is 12 tabs, 1536 channels, 50 sequences, 500 samples per sequence

NOTE: on making the example, using constants instead of parameters helped the compilere ~3 seconds on 50 seconds...

Doing 10 iterations

No optimization: gcc -o naive naive.c

x  ~/Code/gpu_sig $ time ./naive 12 1536 50
  12  1536   50 1757.81MB

  real    0m47.513s
  user    0m46.923s
  sys     0m0.533s

.  ~/Code/gpu_sig $ time ./naive 12 1536 50
  12  1536   50 1757.81MB

  real    0m47.541s
  user    0m46.998s
  sys     0m0.487s



With optimization: gcc -march=native -O3 -Ofast -o naive_optimized naive.c 

.  ~/Code/gpu_sig $ time ./naive_optimized 12 1536 50
  12  1536   50 1757.81MB

  real    0m9.927s
  user    0m9.393s
  sys     0m0.516s

.  ~/Code/gpu_sig $ time ./naive_optimized 12 1536 50
  12  1536   50 1757.81MB

  real    0m9.945s
  user    0m9.399s
  sys     0m0.529s



Scaling up the number of packets:
for i in `seq 1 50`; do  time ./naive 12 1536  $i ; done &> o

Scaling up the number of channels:
for i in `seq 4 4 400`; do  time ./naive 12 $i 50 ; done &> o2
for i in `seq 436 100 1536`; do  time ./naive 12 $i 50 ; done &> o2

