# Codes for Parameter Tests

We compare the performance (average error and throughput) of RingSketch configured with different parameters. The results in Figure 5 are derived from these tests.

## How to run

Build the test with the following command:

```bash
$ make
```

Run the test with the following command:

```bash
$ ./parameter
```

By default, the input comes from `../../dataset/caida.dat` and the output goes to `./parameter.csv`. The paths and filenames can be modified in `./parameter.cpp`:

```cpp
std::string inputFolder = "PATH_OF_DATASETS";
std::string inputFile = "DATASET";
std::string outputFolder = "PATH_OF_RESULTS";
std::string outputFile = "RESULT";
```

We have 5 test cases specified for different aspects of comparison. Uncomment the test case of interest and rebuild the test each time.

The update speed of the clock pointer can be adjusted to raise the throughput at the expense of granularity. Navigate to `../../algos/RingSketch/RingSketch.h` and modify the update speed in the constructor:

```cpp
updateSpeed = DESIRED_SPEED;
```

## Output Format

Our program outputs comma-separated values with the following heading:

```
name,memory,counterNum,counterSize,are,aae
```

or

```
name,memory,counterNum,counterSize,thr
```