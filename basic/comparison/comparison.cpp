#include <random>
#include <cassert>
#include <deque>
#include <iostream>
#include <fstream>
#include "../../algos/Algos.h"

std::string inputFolder = "../../dataset/";
std::string inputFile = "caida.dat";
std::string outputFolder = "./";
std::string outputFile = "comparison.csv";

uint64_t datasetLength;
TUPLES* dataset;

class scheme {
public:
    std::string name;
    scheme(std::string n): name(n) {}
    virtual double measure(int, int) = 0;
};

class ARE: public scheme {
public:
    ARE(): scheme(std::string("ARE")) {}
    double measure(int ans, int truans) {
        double dt = std::abs(ans - truans);
        return dt / truans;
    }
};

class AAE: public scheme {
public:
    AAE(): scheme(std::string("AAE")) {}
    double measure(int ans, int truans) {
        double dt = std::abs(ans - truans);
        return dt;
    }
};

Abstract* getSketch(int window_size, int memory, int counterNum, int counterSize, int sketchtype) {
    Abstract* sketch;

    switch (sketchtype)
    {
    case 1:
        sketch = new RingSketch(window_size, memory, counterNum, counterSize, false);
        break;
    case 2:
        sketch = new RingSketchSimd(window_size, memory, counterNum, counterSize);
        break;
    case 3:
        sketch = new RingSketchThread(window_size, memory, counterNum, counterSize, 1);
        break;
    case 4:
        sketch = new ClockSketch(window_size, memory, 1 + (0.6931 * memory * 8) / (window_size * counterSize), counterSize);
        break;
    case 5:
        sketch = new TimeoutBloomFilter(memory, counterNum);
        break;
    case 6:
        sketch = new HyperBloomFilter(window_size, memory, 8, 8, counterSize);
        break;
    case 7:
        sketch = new SWAMP(memory / 25, memory);
        break;
    }

    return sketch;
}

std::string test(int query_freq, int window_size, int memory, int counterNum, int counterSize, int sketchtype, scheme* measurer) {
    Abstract* sketch;
    uint64_t c = datasetLength;

    double accu = 0;
    double num = 0;

    
    for(int k = 0; k < 4; ++k) {
        srand(131 + k);
        std::deque<std::pair<int, TUPLES>> lastTime;
        sketch = getSketch(window_size, memory, counterNum, counterSize, sketchtype);
        for (int i = 0; i < c; i++) {
            TUPLES tuple = dataset[i];
        
            if (i > 0 && i % query_freq == 0) {
                while (i - lastTime.front().first > window_size)
                    lastTime.pop_front();

                auto random_it = std::next(std::begin(lastTime), rand() % lastTime.size());

                int gt = i - random_it->first;

                for (auto iter = lastTime.rbegin(); iter != lastTime.rend(); ++iter)
                    if (iter->second == random_it->second) {
                        gt = i - iter->first;
                        break;
                    }

                int est = sketch->estimate(random_it->second, i);

                accu += measurer->measure(est, gt);
                num += 1;
            }

            sketch->insert(tuple, i);
            lastTime.emplace_back(i, tuple);
            sketch->updateClock(1);
        }
        delete sketch;
    }

    char returnString[200] = {0};
    sprintf(returnString, ",%.10f", accu / num);
    return std::string(returnString);
}

std::string testTHR(int window_size, int memory, int counterNum, int counterSize, int sketchtype, int test_cycle) {
    Abstract* sketch;

    uint64_t c = datasetLength;
    auto t13 = 0, t23 = 0, t3 = 0;

    for(int i = 0; i < test_cycle; ++i) {
        sketch = getSketch(window_size, memory, counterNum, counterSize, sketchtype);
        auto t1 = std::chrono::steady_clock::now();
        for(int k = 0; k < c; k++){
            TUPLES tuple = dataset[k];
            sketch->insert(tuple, i);
            if(sketchtype != 3)
                sketch->updateClock(1);
        }
        auto t2 = std::chrono::steady_clock::now();
        t3 +=std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        delete sketch;
    }

    char returnString[200] = {0};
    sprintf(returnString, ",%.10f", c * test_cycle / (1.0 * t3));
    return std::string(returnString);
}

std::string getString(int memory, int counterNum, int counterSize, int t) {
    char returnString[200] = {0};
    Abstract *sketch = getSketch(1<<13, memory, counterNum, counterSize, t);
    sprintf(returnString, "%s,%d,%d,%d", sketch->name.c_str(), memory, counterNum, counterSize);
    delete sketch;
    return std::string(returnString);
}

void testCase(std::ofstream &outFile) {
    int memory = 8000;
    int query_freq = 10;
    int window_size = 1 << 13;

    scheme *are = new ARE, *aae = new AAE;

    outFile<<"name,memory,counterNum,counterSize,are,aae,thr"<<std::endl;

    for(int i = 4; i <= 4; i += 1){
        // RingSketch
        outFile<<getString(memory * i, 4, 16, 2);
        outFile<<test(query_freq, window_size, memory * i, 4, 16, 2, are);
        outFile<<test(query_freq, window_size, memory * i, 4, 16, 2, aae);
        outFile<<testTHR(window_size, memory * i, 4, 16, 3, 1);
        outFile<<std::endl;

        // ClockSketch
        outFile<<getString(memory * i, 4, 16, 4);
        outFile<<test(query_freq, window_size, memory * i, 4, 16, 4, are);
        outFile<<test(query_freq, window_size, memory * i, 4, 16, 4, aae);
        outFile<<testTHR(window_size, memory * i, 4, 16, 4, 1);
        outFile<<std::endl;

        // TimeoutBloomFilter
        outFile<<getString(memory * i, 16, 8, 5);
        outFile<<test(query_freq, window_size, memory * i, 16, 8, 5, are);
        outFile<<test(query_freq, window_size, memory * i, 16, 8, 5, aae);
        outFile<<testTHR(window_size, memory * i, 16, 8, 5, 1);
        outFile<<std::endl;

        // HyperBloomFilter
        outFile<<getString(memory * i, 0, 8, 6);
        outFile<<test(query_freq, window_size, memory * i, 0, 8, 6, are);
        outFile<<test(query_freq, window_size, memory * i, 0, 8, 6, aae);
        outFile<<testTHR(window_size, memory * i, 0, 8, 6, 1);
        outFile<<std::endl;

        // SWAMP
        outFile<<getString(memory * i, 0, 8, 7);
        outFile<<test(query_freq, window_size, memory * i, 0, 8, 7, are);
        outFile<<test(query_freq, window_size, memory * i, 0, 8, 7, aae);
        outFile<<testTHR(window_size, memory * i, 0, 8, 7, 1);
        outFile<<std::endl;
    }

    delete are;
    delete aae;
}

int main(int argc,char* argv[]) {

    dataset = read_data((inputFolder + inputFile).c_str(), 1 << 20, &datasetLength).first;

    std::ofstream outFile(outputFolder + outputFile);

    testCase(outFile);

    outFile.close();
    return 0;
}
