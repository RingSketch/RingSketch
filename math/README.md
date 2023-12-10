# Codes for Math Tests

We compare the theoretical analysis and practical performance of RingSketch on Zipf distribution. The results in Figure 3 are derived from these tests.

## How to run

Build the test with the following command:

```bash
$ make
```

Run the test with the following command:

```bash
$ ./math
```

The dataset in use should be specified in `./math.cpp`. For example, we take `../../dataset/zipf_1.0.dat` as input.

```cpp
std::string folder = "../dataset/";
std::string file[1] = {"zipf_1.0.dat"};

int main(int argc,char* argv[]) {
    dataset = read_data_zipf((folder + file[0]).c_str(), 1 << 22, &datasetLength);

    ...
}
```

If you want to test the results under `zipf 1.5``, you need to modify the alpha value and calculation formula of the theoretical calculation functions in addition to the dataset.

```cpp
printf("%.10f,", calcLowerBound(1.5, window_size, window_size, sketch->counterInterval, sketch->counterNum, sketch->updateSpeed));
printf("%.10f\n", calcUpperBound(1.5, window_size, window_size, sketch->counterInterval, sketch->counterNum, sketch->updateSpeed));
```

## Output Format

Our program outputs comma-separated values with the following heading:

```
name,memory,counterNum,counterSize,aae,theoretical (collision-free),upperbound
```

which specifies the algorithm, memory usage, parameters, practical performance and theoretical analysis in every single test case.

