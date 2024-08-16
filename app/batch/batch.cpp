#include <random>
#include <cassert>
#include <deque>
#include <map>
#include <set>
#include <unordered_set>
#include "../../algos/Algos.h"

uint64_t datasetLength;
std::pair<TUPLES*, TIMESTAMP*> dataset;

Abstract* chooseSketch(int sketchtype, int window_size, int memory, int counterNum, int counterSize) {
    switch (sketchtype)
    {
    case 0:
        return new RingSketch(window_size, memory, counterNum, counterSize);
    case 1:
        return new TimeoutBloomFilter(memory, counterNum);
    case 2:
        return new HyperBloomFilter(window_size, memory, counterNum, 3, counterSize);
    case 3:
        return new ClockSketch(window_size, memory, counterNum, counterSize);
    }
    abort();
}

std::pair<std::string, std::vector<int>> testBatchEst(int type, const int window_size, const int memory, int counterNum, int counterSize, int timeThreshold) {
    std::vector<int> results;

    Abstract *sketch = chooseSketch(type, window_size, memory, counterNum, counterSize);

    double start = dataset.second[0];
    for (int i = 0; i < datasetLength; i++) {
        TUPLES tuple = dataset.first[i];
        int timestamp = i;
        
        if (sketch->estimate(tuple, timestamp) > timeThreshold)
            results.push_back(i);

        sketch->insert(tuple, timestamp);
        sketch->updateClock(1);
    }

    return std::make_pair(sketch->name, results);
}

int single_hit_test(
    const std::vector<int>& results,
    const std::vector<int>& batches
) {
    int correct_count = 0;
    int j = 0;
    for (int i : results) {
        while (j + 1 < int(batches.size()) && batches[j] < i)
            ++j;
        if (j < int(batches.size()) && batches[j] == i) {
            ++correct_count;
        }
    }
    return correct_count;
}

std::string folder = "../../dataset/";

std::string file[1] = {"zipf_1.0.dat"};

int main(int argc,char* argv[]) {
    srand(233);

    dataset = read_data_zipf((folder + file[0]).c_str(), 1 << 22, &datasetLength);

    int counterSize = 8;
    int memory = 5000;

    int query_freq = 1;
    int window_size = 1 << 14;

    int timeThreshold = 50;

    auto testBatch = [&](int limNum, int limT) {
        for (int counterNum = 4; counterNum <= limNum; counterNum += 2)
            for (int j = 2; j <= 6; j++) {
                int nowThreshold = timeThreshold * j;

                std::unordered_map<TUPLES, int> lastTime;
                std::unordered_map<TUPLES, int> batchSize;
                std::vector<int> batches;

                for (int i = 0; i < datasetLength; i++) {
                    TUPLES tuple = dataset.first[i];
                    int timestamp = i;

                    if (lastTime.count(tuple) == false || timestamp - lastTime[tuple] > nowThreshold) {
                        batchSize[tuple] = 1;
                    }else {
                        batchSize[tuple] += 1;
                    }

                    if (batchSize[tuple] == 1)
                        batches.push_back(i);

                    lastTime[tuple] = timestamp;
                }

                for (int t = 0; t < limT; t++) {
                    auto [name, results] = testBatchEst(t, window_size, memory, counterNum, counterSize, nowThreshold);

                    double real_count = batches.size();
                    double est_count = results.size();
                    double correct_count = single_hit_test(results, batches);

                    double recall = correct_count / real_count;
                    double precision = correct_count / est_count;
                    
                    double f1 = recall || precision ? 2 * recall * precision / (recall + precision) : 0.;

                    printf("%s,%d,%d,%d,%d,%d,%.6f,%.6f,%.6f\n", name.c_str(), window_size, nowThreshold, memory, counterNum, counterSize, recall, precision, f1);
                }
            }
    };
    
    freopen("./result_batch_different_algo.csv", "w", stdout);
    printf("name,windowSize,threshold,memory,counterNum,counterSize,rr,pr,f1\n");
    testBatch(4, 4);
    
    freopen("./result_batch_different_counterNum.csv", "w", stdout);
    printf("name,windowSize,threshold,memory,counterNum,counterSize,rr,pr,f1\n");
    testBatch(10, 1);
    return 0;
}
