#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#include "constants.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Need 3 arguments: ntabs, nchannels, npackets\n");
    exit(EXIT_FAILURE);
  }

  int ntabs = atoi(argv[1]);
  int nchannels = atoi(argv[2]);
  int npackets = atoi(argv[3]);

  size_t mysize = ntabs * nchannels * NSAMPS * npackets * NPOLS;
  printf("% 4i % 4i % 4i %6.2fMB\n", ntabs, nchannels, npackets, mysize / (1024.0*1024.0));

  char *transposed = (char *)malloc(mysize);
  char *page = (char *)malloc(mysize);

  double start = omp_get_wtime();

  int i;
  for (i=0; i<10; i++) {
    deinterleave(page, transposed, ntabs, nchannels, npackets);
    // memcpy(page, transposed, mysize);
  }

  double end = omp_get_wtime();

  printf("Average time per call: %.6f ms\n", (end - start)*1e3/10);

  free(page);
  free(transposed);

  exit(EXIT_SUCCESS);
}
