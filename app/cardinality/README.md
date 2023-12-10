# Codes for Cardinality Tests

We compare the performance (Absolute Error, Relative Error) of different algorithms. The results in Figure 11, 12 are derived from these tests.

## How to run

Build the test with the following command:

```bash
$ make
```

Run the test with the following command:

```bash
$ ./cardinality
```

The dataset in use should be specified in `./cardinality.cpp`. For example, we take `../../dataset/zipf_1.0.dat` as input.

```cpp
std::string folder = "../../dataset/";

std::string file[1] = {"zipf_1.0.dat"};

int main(int argc,char* argv[]) {
    dataset = read_data_zipf((folder + file[0]).c_str(), 1 << 22, &datasetLength);

    ...
}
```

## Output Format

Our program will generate a output file with the following heading:

```
name,windowSize,threshold,memory,counterNum,counterSize,are,aae
```

which specifies the algorithm, memory usage, window size, parameters and performance achieved in every single test case.
