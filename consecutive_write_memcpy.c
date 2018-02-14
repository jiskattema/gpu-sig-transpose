#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"

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
 *  @param {const char *} page    Ringbuffer page with interleaved data
 *  @param {const char *} transposed
 *  @param {int} ntabs                     Number of tabs
 *  @param {int} nchannels                 Number of channels
 *  @param {int} npackets                  Number of packets per sequence
 */
void deinterleave (const unsigned char *page, unsigned char *transposed, const int ntabs, const int nchannels, const int npackets) {
  const unsigned char *packet = page;

  int tab = 0;
  for (tab = 0; tab < ntabs; tab++) {

    int channel_offset = 0;
    for (channel_offset = 0; channel_offset < nchannels; channel_offset+=4) {

      const unsigned char *src = &page[(tab * nchannels + channel_offset)*NPOLS*npackets*NSAMPS];
      unsigned char *dest = &transposed[(tab * nchannels + channel_offset)*NPOLS*npackets*NSAMPS];

      int pc = 0;

#pragma omp parallel for
      for (pc=0; pc < 16; pc+=4) {
        unsigned char temp[npackets*NSAMPS*4];

        unsigned char *tempA = &temp[0 * npackets*NSAMPS];
        unsigned char *tempB = &temp[1 * npackets*NSAMPS];
        unsigned char *tempC = &temp[2 * npackets*NSAMPS];
        unsigned char *tempD = &temp[3 * npackets*NSAMPS];

        int tn = 0;
        for (tn = 0; tn < npackets*NSAMPS; tn++) {
          tempA[tn] = src[tn*NPOLS*NCHANS + pc + 0];
          tempB[tn] = src[tn*NPOLS*NCHANS + pc + 1];
          tempC[tn] = src[tn*NPOLS*NCHANS + pc + 2];
          tempD[tn] = src[tn*NPOLS*NCHANS + pc + 3];
        }

        memcpy(&dest[pc*npackets*NSAMPS], temp, 4*npackets*NSAMPS);
      }

    } // channel_offset
  } // tab
}
