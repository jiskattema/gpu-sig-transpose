#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <omp.h>

#define NPOLS 4
#define NCHANS 4
#define NSAMPS 500

/* Lower bound on timings by a single memcpy
 * gives ~1.19 seconds for 10 iterations, 1757 MB  /0.11s ~= 1600 MB/s
   [root@laptopjisk ~]# dmidecode -t 17
   # dmidecode 3.1
   Getting SMBIOS data from sysfs.
   SMBIOS 3.0.0 present.

   Handle 0x0039, DMI type 17, 40 bytes
   Memory Device
               Array Handle: 0x0038
               Error Information Handle: Not Provided
               Total Width: 64 bits
               Data Width: 64 bits
               Size: 8192 MB
               Form Factor: Row Of Chips
               Set: None
               Locator: System Board Memory
               Bank Locator: BANK 0
               Type: LPDDR3
               Type Detail: Synchronous Unbuffered (Unregistered)
               Speed: 1867 MT/s
               Manufacturer: Micron
               Serial Number: 00000000
               Asset Tag: 9876543210
               Part Number: MT52L1G32D4PG-107 
               Rank: 2
               Configured Clock Speed: 1867 MT/s
               Minimum Voltage: 1.25 V
               Maximum Voltage: 1.25 V
               Configured Voltage: 1.2 V
*/


/**
 * Deinterleave (transpose) an IQUV ring buffer page to the ordering needed for FITS files
 * Note that this is probably a slow function, and is not meant to be run real-time
 *
 *  data in:   tab, channel/4, time/500 packets of time,channel,pn
 *  data  out: tab, channel, pol, time
 *
 * Suggested use is:
 *   1. realtime: ringbuffer -> [trigger] -> dada_dbdisk
 *   2. offline: dada_dbdisk -> ringbuffer -> dadafits
 *
 *  @param {const unsigned char *} page    Ringbuffer page with interleaved data
 *  @param {int} ntabs                     Number of tabs
 *  @param {int} nchannels                 Number of channels
 *  @param {int} npackets                  Number of packets per sequence
 */
float deinterleave (char *transposed, const char *page, const int ntabs, const int nchannels, const int npackets) {
  const char *packet = page;
  double start = omp_get_wtime();

  // and find the matching address in the transposed buffer
  int tab = 0;
  for (tab = 0; tab < ntabs; tab++) {
    int channel_offset = 0;
    for (channel_offset = 0; channel_offset < nchannels; channel_offset+=4) {
      int sequence_number = 0;
      for (sequence_number = 0; sequence_number < npackets; sequence_number++) {
        // process packet
        int tn,cn,pn;
        for (tn = 0; tn < NSAMPS; tn++) {      // 500 samples per packet
          for (cn = 0; cn < NCHANS; cn++) {    // 4 channels per packet
            for (pn = 0; pn < NPOLS; pn++) {   // 4 poliarizations per packet
              transposed[
                ((tab * nchannels + cn + channel_offset) * NPOLS + pn) * npackets * NSAMPS +
                  tn + sequence_number * NSAMPS 
              ] = *packet;
            }
          }
        }
      }
    }
  }

  return (float)((omp_get_wtime() - start)*1e3);
}

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
    deinterleave(transposed, page, ntabs, nchannels, npackets);
    // memcpy(page, transposed, mysize);
  }

  double end = omp_get_wtime();

  printf("time per call: %.6f ms\n", (end - start)*1e3/10.0);

  free(page);
  free(transposed);

  exit(EXIT_SUCCESS);
}
