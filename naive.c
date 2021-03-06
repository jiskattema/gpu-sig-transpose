#include "constants.h"

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
void deinterleave(unsigned char * restrict const page, unsigned char * restrict const transposed, const int ntabs, const int nchannels, const int npackets) {
  // and find the matching address in the transposed buffer
  const int ni = ntabs * nchannels / NCHANS;
  const int nj = npackets * NSAMPS;
  const int nk = NCHANS * NPOLS;

  int i = 0;
  #pragma omp parallel for
  for (i = 0; i < ni; i++) {
    int j;
    for (j = 0; j < nj; j++) {
      int k = 0;
      for (k = 0; k < nk; k++) {
        transposed[(i * nk + k) * nj + j] = page[(i * nj + j) * nk + k];
      }
    }
  }
}
