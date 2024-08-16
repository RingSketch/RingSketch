## Datasets

We conduct experiments under two real-world datasets and one synthetic dataset.

1) CAIDA dataset:
   CAIDA is a collection of IP trace datasets collected on backbone links by CAIDA 2018. 
   We treat each packet in the traces as one item. In our experiments, we use a 1-minute IP trace containing about 30M items derived from 1.3M distinct items. 

2) Criteo dataset:
   Criteo is an advertising click data stream consisting of feature values and click feedback for many display ads.
   For each ad, we use the hash value (8 bytes) of its categorical features as the ID field. 
   We use a dataset containing about 48M items derived from 2.4M distinct ones. 

3) Zipf dataset:
   We use Web Polygraph to generate datasets according to Zipf distribution. We generate multiple Zipf datasets with different $\alpha$, which reflects the degree of skewness of the distribution. 
   Each dataset consists of 32M items.

We provide four small sample datasets extracted from the three datasets in the `dataset` folder to run our code properly. For the full datasets, please download them from [CAIDA](https://www.caida.org/), [Criteo](https://ailab.criteo.com/download-criteo-1tb-click-logs-dataset/), [Zipf 1.0](https://drive.google.com/file/d/1-OTs5BYaK6UB26F0xKsOwnA00cjxDt26/view?usp=drive_link), and [Zipf 1.5](https://drive.google.com/file/d/1GfqimNTlG3RnTFClKdcGfS-lKq5dH3ED/view?usp=drive_link).
