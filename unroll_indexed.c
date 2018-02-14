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

  int packets_processed = 0;

  int tab = 0;
  for (tab = 0; tab < ntabs; tab++) {

    int channel_offset = 0;
    for (channel_offset = 0; channel_offset < nchannels; channel_offset+=4) {

      unsigned char *dest = &transposed[(tab * nchannels + channel_offset)*NPOLS*npackets*NSAMPS];
      const unsigned char *src = &page[packets_processed * NPOLS * NCHANS * NSAMPS];

      int sequence_number = 0;
      for (sequence_number = 0; sequence_number < npackets; sequence_number++) {

        // process packet:
        int tn;
#pragma omp parallel for
        for (tn = 0; tn < NSAMPS; tn++) {      // 500 samples per packet
          dest[( 0 * NPOLS +  0) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 0 * NPOLS + 0];
          dest[( 0 * NPOLS +  1) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 0 * NPOLS + 1]; 
          dest[( 0 * NPOLS +  2) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 0 * NPOLS + 2]; 
          dest[( 0 * NPOLS +  3) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 0 * NPOLS + 3]; 
          dest[( 1 * NPOLS +  0) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 1 * NPOLS + 0]; 
          dest[( 1 * NPOLS +  1) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 1 * NPOLS + 1]; 
          dest[( 1 * NPOLS +  2) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 1 * NPOLS + 2]; 
          dest[( 1 * NPOLS +  3) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 1 * NPOLS + 3]; 
          dest[( 2 * NPOLS +  0) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 2 * NPOLS + 0]; 
          dest[( 2 * NPOLS +  1) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 2 * NPOLS + 1]; 
          dest[( 2 * NPOLS +  2) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 2 * NPOLS + 2]; 
          dest[( 2 * NPOLS +  3) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 2 * NPOLS + 3]; 
          dest[( 3 * NPOLS +  0) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 3 * NPOLS + 0]; 
          dest[( 3 * NPOLS +  1) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 3 * NPOLS + 1]; 
          dest[( 3 * NPOLS +  2) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 3 * NPOLS + 2]; 
          dest[( 3 * NPOLS +  3) * npackets * NSAMPS + tn] = src[sequence_number * NPOLS * NCHANS * NSAMPS + tn * NCHANS * NPOLS + 3 * NPOLS + 3]; 
        } // tn

      } // sequence number
      packets_processed += npackets;

    } // channel_offset

  } // tab
}
