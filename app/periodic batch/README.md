# Codes for Periodic Batch Tests

We compare the performance (Absolute Error, Relative Error) of different algorithms on mining periodic items. The results in Figure 11, 12 are derived from these tests.

## How to run

Build the test with the following command:

```bash
$ make
```

Run the test with the following command:

```bash
$ ./periodic
```

The dataset in use should be specified in `./periodic.cpp`. For example, we take `../../dataset/zipf_1.0.dat` as input.

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
name,memory,aae,are,rr,pr
```

which specifies the algorithm, memory usage, window size and performance achieved in every single test case.
