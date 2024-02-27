# Codes for Comparison Tests

We compare the performance (average error and throughput) of different algorithms. The results in Figure 6, 7 and 8 are derived from these tests.

## How to run

Build the test with the following command:

```bash
$ make
```

Run the test with the following command:

```bash
$ ./comparison
```

The dataset in use should be specified in `./comparison.cpp`. For example, we take `../../dataset/caida.dat` as input and outputs the result at `./comparison.csv`.

```cpp
std::string inputFolder = "../../dataset/";
std::string inputFile = "caida.dat";
std::string outputFolder = "./";
std::string outputFile = "comparison.csv";

int main() {
    dataset = read_data((inputFolder + inputFile).c_str(), 1 << 20, &datasetLength).first;

    std::ofstream outFile(outputFolder + outputFile);
    ...
}
```

## Output Format

Our program outputs comma-separated values with the following heading:

```
name,memory,counterNum,counterSize,are,aae,thr
```

which specifies the algorithm, memory usage, parameters and performance achieved in every single test case.

