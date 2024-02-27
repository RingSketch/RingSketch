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
    switch (sketchtype) {
    case 0:
        return new RingSketch(window_size, memory, counterNum, counterSize);
    case 1:
        return new TimeoutBloomFilter(memory, counterNum);
    case 2:
        return new HyperBloomFilter(window_size, memory, counterNum, 3, counterSize);
    }
    abort();
}

void testCardinalEst(int type, const int window_size, int threshold, const int memory, int counterNum, int counterSize)
{
    Abstract *sketch = chooseSketch(type, window_size, memory, counterNum, counterSize);

    int T = datasetLength / window_size;
    double RE[T] = {0}, ARE = 0, AAE = 0;
    for(int win_cnt = 0; win_cnt < T; ++win_cnt) {
        std::unordered_set<TUPLES> inSet;
        for(int i = win_cnt * window_size; i < (win_cnt + 1) * window_size; ++i) {
            TUPLES ID = dataset.first[i];

            sketch->insert(ID, i);
            sketch->updateClock(1);

            if (i >= (win_cnt + 1) * window_size - threshold)
                inSet.insert(ID);
        }
        double real_card = (double)inSet.size();
        double bm_card = sketch->cardinalEst(threshold, (win_cnt + 1) * window_size - 1);
        RE[win_cnt] = fabs(1 - bm_card / real_card);
        ARE += RE[win_cnt];
        AAE += fabs(bm_card - real_card);
    }
    printf("%s,%d,%d,%d,%d,%d,%.6f,%.6f\n", sketch->name.c_str(), window_size, threshold, memory, counterNum, counterSize, ARE / T, AAE / T);
}

std::string folder = "../../dataset/";

std::string file[1] = {"zipf_1.0.dat"};

int main(int argc,char* argv[]) {
    srand(233);

    dataset = read_data_zipf((folder + file[0]).c_str(), 1 << 22, &datasetLength);

    int depth = 1;
    int memory = 5000;

    int query_freq = 1;
    int window_size = 1 << 14;

    freopen("result_cardinality.csv", "w", stdout);
    printf("name,windowSize,threshold,memory,counterNum,counterSize,are,aae\n");
    for (int j = 4; j <= 8; j++)
        for (int t = 0; t < 3; t++)
            testCardinalEst(t, window_size, j * 500, memory, depth, 8);
    
    return 0;
}
