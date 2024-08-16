# Measuring Item Freshness in Data Streams

This repository contains all related code of our paper "Measuring Item Freshness in Data Streams".

Our codes can automatically reproduce all experimental results in our paper.

## Overview

We implement and test RingSketch on a CPU platform (Intel i9-10980XE, 18-core 4.2 GHz CPU with 128GB 3200 MHz DDR4 memory and 24.75 MB L3 cache) and implement RingSketch on top of Apache Flink. We also implement four existing algorithms: ClockSketch, HyperBloomFilter, SWAMP, TimeoutBloomFilter. 

## Dependencies

- g++ 7.5.0 (Ubuntu 7.5.0-6ubuntu2)
- BOB Hash
- [Optional] AVX-256 Intel Intrinsics (SIMD operations)

## File structures

- `algos`: All the algorithms we implemented and their required structures
    - `Abstract.h`: Implementation of the base class for algorithms
    - `Algos.h`: Header file that includes all algorithm implementations for easy reference in experimental code
    - `BitMap.h` & `CuckooMap.h`: CuckooMap implementation for the SWAMP algorithm
    - `hash.h`: Implementation of the BOB Hash algorithm
    - `Util.h`: Includes dataset reading operations and definitions of some basic data types
    - `Cache/`: Contains all the algorithms tested in the `cache` experiment
        - `RingSketchCacheVer0.h`: Implementation of RingCache
        - `RingSketchCacheVer0Thread.h`: Implementation of the multi-threaded version of RingCache
        - `LRU.h`: Implementation of the LRU algorithm
    - `Comparison/`: Contains implementations of all the comparative algorithms
        - `ClockSketch.h`: Implementation of the ClockSketch algorithm
        - `HyperBloomFilter.h`: Implementation of the HyperBloomFilter algorithm
        - `SWAMP.h`: Implementation of the SWAMP algorithm
        - `TimeoutBloomFilter.h`: Implementation of the TimeoutBloomFilter algorithm
    - `RingSketch/`: Contains different implementations of RingSketch
        - `RingSketch.h`: The final implementation of RingSketch
        - `RingSketchSimd.h`: SIMD optimized version of RingSketch
        - `RingSketchThread.h`: Multi-thread optimized version of RingSketch
        - `RingSketchIdentical.h`: RingSketch version with uniformly distributed positions
- `basic`: Contains comparisons between different implementation methods of RingSketch and comparisons between RingSketch and other algorithms
    - `comparison`: Contains experiments comparing RingSketch with other algorithms
    - `parameter`: Contains experiments comparing different implementation methods of RingSketch
- `app`: Performance testing of RingSketch applications
    - `cache`: Performance testing of RingCache
    - `batch`: Comparative experiments between RingSketch and other algorithms in terms of batch processing performance
    - `cardinality`: Comparative experiments between RingSketch and other algorithms in terms of item cardinality estimation performance
    - `periodic batch`: Comparative experiments between RingSketch and Periodic Sketch in terms of periodic element detection performance
- `math`: Theoretical analysis and practical performance comparison of RingSketch on Zipf distribution
- `flink`: Implementation of RingSketch on the Flink platform
- `dataset`: Contains the cropped results of the CAIDA, Criteo, Zipf_1.0 and Zipf_1.5 datasets we used

## Datasets

We conduct experiments under two real-world datasets and one synthetic dataset.

1. CAIDA dataset:

  CAIDA is a collection of IP trace datasets collected by CAIDA 2018. We treat each packet as one item, and use its 13-byte 5-tuple (source/destination IP, source/destination port number, protocol type) as ID. By default, we use a small-scale 1-minute trace containing about 30M items (with 1.3M distinct items). We also use a large-scale 1-hour trace containing 1.5G items (with 60M distinct items) to demonstrate the scalability of RingSketch.

2. Criteo dataset:

  Criteo is an advertising click data stream consisting of feature values and click feedback for many display ads.
  For each ad, we use the hash value (8 bytes) of its categorical features as the ID field. 
  We use a dataset containing about 48M items derived from 2.4M distinct ones. 

3. Zipf dataset:

  We use Web Polygraph to generate datasets according to Zipf distribution. We generate multiple Zipf datasets with different $\alpha$, which reflects the degree of skewness of the distribution. 
  Each dataset consists of 32M items.

We provide four small sample datasets extracted from the three datasets in the `dataset` folder to run our code properly. For the full datasets, please download them from [CAIDA](https://www.caida.org/), [Criteo](https://ailab.criteo.com/download-criteo-1tb-click-logs-dataset/), [Zipf 1.0](https://drive.google.com/file/d/1-OTs5BYaK6UB26F0xKsOwnA00cjxDt26/view?usp=drive_link), and [Zipf 1.5](https://drive.google.com/file/d/1GfqimNTlG3RnTFClKdcGfS-lKq5dH3ED/view?usp=drive_link).

## How to run

We provide the detailed instructions on how to run the codes for specific experiments in corresponding folders.
