# Codes for Batch Tests

We compare the performance (F1 score) of different algorithms on mining batches. The results in Figure 13 are derived from these tests.

## How to run

Build the test with the following command:

```bash
$ make
```

Run the test with the following command:

```bash
$ ./batch
```

The dataset in use should be specified in `./batch.cpp`. For example, we take `../../dataset/zipf_1.0.dat` as input.

```cpp
std::string folder = "../../dataset/";
std::string file[1] = {"zipf_1.0.dat"};

int main(int argc, char* argv[]) {
    dataset = read_data_zipf((folder + file[0]).c_str(), 1 << 22, &datasetLength);

    ...
}
```

## Output Format

Our program outputs comma-separated values with the following heading:

```
name,windowSize,threshold,memory,counterNum,counterSize,rr,pr,f1
```

which specifies the algorithm, memory usage, batch threshold, parameters and performance achieved in every single test case.

