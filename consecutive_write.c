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

      int pc;
      for (pc=0; pc < 16; pc+=4) {
        int tn = 0;
#pragma omp parallel for
        for (tn = 0; tn < npackets*NSAMPS; tn++) {
          unsigned int data = *((unsigned int *) &src[tn*NPOLS*NCHANS + pc]);
          dest[tn + (pc + 0) * npackets*NSAMPS] = (data >>  0) && 255; // src[tn*NPOLS*NCHANS +  0];
          dest[tn + (pc + 1) * npackets*NSAMPS] = (data >>  8) && 255; // src[tn*NPOLS*NCHANS +  1];
          dest[tn + (pc + 2) * npackets*NSAMPS] = (data >> 16) && 255; // src[tn*NPOLS*NCHANS +  2];
          dest[tn + (pc + 3) * npackets*NSAMPS] = (data >> 24) && 255; // src[tn*NPOLS*NCHANS +  3];
        }
      }
    } // channel_offset
  } // tab
}


/* Manually unrolling the loop over pc gives slight speedups; code below for reference
 *
      int tn = 0;
      for (tn = 0; tn < npackets*NSAMPS; tn++) {
        unsigned int data = *((unsigned int *) &src[tn*NPOLS*NCHANS]);
        dest[tn +  0 * npackets*NSAMPS] = (data >>  0) && 255; // src[tn*NPOLS*NCHANS +  0];
        dest[tn +  1 * npackets*NSAMPS] = (data >>  8) && 255; // src[tn*NPOLS*NCHANS +  1];
        dest[tn +  2 * npackets*NSAMPS] = (data >> 16) && 255; // src[tn*NPOLS*NCHANS +  2];
        dest[tn +  3 * npackets*NSAMPS] = (data >> 24) && 255; // src[tn*NPOLS*NCHANS +  3];
      }

      for (tn = 0; tn < npackets*NSAMPS; tn++) {
        unsigned int data = *((unsigned int *) &src[tn*NPOLS*NCHANS + 4]);
        dest[tn +  4 * npackets*NSAMPS] = (data >>  0) && 255; // src[tn*NPOLS*NCHANS +  4];
        dest[tn +  5 * npackets*NSAMPS] = (data >>  8) && 255; // src[tn*NPOLS*NCHANS +  5];
        dest[tn +  6 * npackets*NSAMPS] = (data >> 16) && 255; // src[tn*NPOLS*NCHANS +  6];
        dest[tn +  7 * npackets*NSAMPS] = (data >> 24) && 255; // src[tn*NPOLS*NCHANS +  7];
      };

      for (tn = 0; tn < npackets*NSAMPS; tn++) {
        unsigned int data = *((unsigned int *) &src[tn*NPOLS*NCHANS + 8]);
        dest[tn +  8 * npackets*NSAMPS] = (data >>  0) && 255; // src[tn*NPOLS*NCHANS +  8];
        dest[tn +  9 * npackets*NSAMPS] = (data >>  8) && 255; // src[tn*NPOLS*NCHANS +  9];
        dest[tn + 10 * npackets*NSAMPS] = (data >> 16) && 255; // src[tn*NPOLS*NCHANS + 10];
        dest[tn + 11 * npackets*NSAMPS] = (data >> 24) && 255; // src[tn*NPOLS*NCHANS + 11];
      };

      for (tn = 0; tn < npackets*NSAMPS; tn++) {
        unsigned int data = *((unsigned int *) &src[tn*NPOLS*NCHANS] + 12);
        dest[tn + 12 * npackets*NSAMPS] = (data >>  0) && 255; // src[tn*NPOLS*NCHANS + 12];
        dest[tn + 13 * npackets*NSAMPS] = (data >>  8) && 255; // src[tn*NPOLS*NCHANS + 13];
        dest[tn + 14 * npackets*NSAMPS] = (data >> 16) && 255; // src[tn*NPOLS*NCHANS + 14];
        dest[tn + 15 * npackets*NSAMPS] = (data >> 24) && 255; // src[tn*NPOLS*NCHANS + 15];
      }

      */
