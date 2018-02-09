#!/usr/bin/env python

import logging

import numpy as np

import kernel_tuner as kt

NPOLS = 4
NCHANS = 4
NSAMPS = 500

def test_naive():

    ntabs = np.int32(12)
    nchannels = np.int32(1536)
    npackets = np.int32(50)

    size = ntabs * nchannels * NSAMPS * npackets * NPOLS

    page = np.frombuffer(np.random.bytes(size), dtype=np.int8)
    transposed = np.zeros_like(page)

    args = [transposed, page, ntabs, nchannels, npackets]

    print("without compiler options")
    kt.tune_kernel("deinterleave", "naive.c", 1, args, {}, lang="C")

    cp=['-march=native','-O3','-Ofast']
    kt.tune_kernel("deinterleave", "naive.c", 1, args, {}, lang="C", compiler_options=cp, iterations=10)



if __name__ == "__main__":

    test_naive()
