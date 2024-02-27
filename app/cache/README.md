# Codes for Cache Tests

We compare the performance (LRU Similarity, Hit Rate, Throughput) of RingCache. The results in Figure 9, 10 are derived from these tests.

## How to run

Build the test with the following command:

```bash
$ make
```

Run the test with the following command:

```bash
$ ./cache
```

The dataset in use should be specified in `./cache.cpp`. For example, we take `../../dataset/zipf_1.0.dat` as input.

```cpp
std::string folder = "../../dataset/";
std::string file[1] = {"zipf_1.0.dat"};

int main(int argc,char* argv[]) {
    datasetPair = read_data_zipf((folder + file[0]).c_str(), 1 << 20, &c);

    ...
}
```

## Output Format

Our program will generate two output files with the following heading:

```
cacheSize,bucketDepth,similarity,hitRate
```

and

```
cacheSize,bucketDepth,throughput
```

which specifies the cacheSize, parameters and performance achieved in every single test case.

