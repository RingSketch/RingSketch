# Measuring Item Freshness in Data Streams

This repository contains all related code of our paper "Measuring Item Freshness in Data Streams".

Our codes can automatically reproduce all experimental results in our paper.

## Overview

We implement and test RingSketch on a CPU platform (Intel i9-10980XE, 18-core 4.2 GHz CPU with 128GB 3200 MHz DDR4 memory and 24.75 MB L3 cache) and implement RingSketch on top of Apache Flink. We also implement four existing algorithms: ClockSketch, HyperBloomFilter, SWAMP, TimeoutBloomFilter. 

## Dependencies

- g++ 7.5.0 (Ubuntu 7.5.0-6ubuntu2)
- BOB Hash
- [Optional] AVX-256 Intel Intrinsics (SIMD operations)

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
