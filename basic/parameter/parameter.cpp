#include <random>
#include <cassert>
#include <deque>
#include <iostream>
#include <fstream>
#include "../../algos/Algos.h"

std::string inputFolder = "../../dataset/";
std::string inputFile = "caida.dat";
std::string outputFolder = "./";
std::string outputFile = "parameter.csv";

uint64_t datasetLength;
std::pair<TUPLES*, TIMESTAMP*> dataset;

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
        sketch = new RingSketchThread(window_size, memory, counterNum, counterSize);
        break;
    case 4:
        sketch = new RingSketchIdentical(window_size, memory, counterNum, counterSize);
        break;
    }

    return sketch;
}

std::string test(int query_freq, int window_size, int memory, int counterNum, int counterSize, int sketchtype, scheme* measurer) {
    Abstract* sketch;
    uint64_t c = datasetLength;

    double accu = 0;
    double num = 0;

    srand(131);
    for(int k = 0; k < 10; ++k) {
        std::deque<std::pair<int, TUPLES>> lastTime;
        sketch = getSketch(window_size, memory, counterNum, counterSize, sketchtype);
        for (int i = 0; i < c; i++) {
            TUPLES tuple = dataset.first[i];
        
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
    auto t3 = 0;

    for(int i = 0; i < test_cycle; ++i) {
        sketch = getSketch(window_size, memory, counterNum, counterSize, sketchtype);
        auto t1 = std::chrono::steady_clock::now();
        for(int k = 0; k < c; k++){
            TUPLES tuple = dataset.first[k];
            sketch->insert(tuple, i);
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
    Abstract *sketch = getSketch(1<<10, memory, counterNum, counterSize, t);
    sprintf(returnString, "%s,%d,%d,%d", sketch->name.c_str(), memory, counterNum, counterSize);
    delete sketch;
    return std::string(returnString);
}

void testCase1(std::ofstream &outFile) {
    scheme *are = new ARE, *aae = new AAE;

    int memory = 8000;
    int query_freq = 10;
    int window_size = 1 << 13;

    outFile<<"name,memory,counterNum,counterSize,are,aae"<<std::endl;
    for (int i = 4; i <= 4; i += 1)
        for(int counterSize = 8; counterSize <= 8; counterSize += 4){
    // for (int i = 4; i <= 16; i += 1)
    //     for(int counterSize = 8; counterSize <= 20; counterSize += 4){
            outFile<<getString(memory * i, 4, counterSize, 1);
            outFile<<test(query_freq, window_size, memory * i, 4, counterSize, 1, are);
            outFile<<test(query_freq, window_size, memory * i, 4, counterSize, 1, aae);
            outFile<<std::endl;
        }
    
    delete are;
    delete aae;
}

void testCase2(std::ofstream &outFile) {
    scheme *are = new ARE, *aae = new AAE;

    int memory = 8000;
    int query_freq = 10;
    int window_size = 1 << 13;

    outFile<<"name,memory,counterNum,counterSize,are,aae"<<std::endl;
    for(int i = 4; i <= 16; i += 1)
        for(int counterNum = 4; counterNum <= 16; counterNum += 4) {
            outFile<<getString(memory * i, counterNum, 16, 2);
            outFile<<test(query_freq, window_size, memory * i, counterNum, 16, 2, are);
            outFile<<test(query_freq, window_size, memory * i, counterNum, 16, 2, aae);
            outFile<<std::endl;
        }

    delete are;
    delete aae;
}

void testCase3(std::ofstream &outFile) {
    scheme *are = new ARE, *aae = new AAE;

    int memory = 8000;
    int query_freq = 10;
    int window_size = 1 << 13;

    outFile<<"name,memory,counterNum,counterSize,are,aae"<<std::endl;
    for(int i = 4; i <= 16; i += 1) {
        // Independent
        outFile<<getString(memory * i, 4, 16, 2);
        outFile<<test(query_freq, window_size, memory * i, 4, 16, 2, are);
        outFile<<test(query_freq, window_size, memory * i, 4, 16, 2, aae);
        outFile<<std::endl;

        // Identical
        outFile<<getString(memory * i, 4, 16, 4);
        outFile<<test(query_freq, window_size, memory * i, 4, 16, 4, are);
        outFile<<test(query_freq, window_size, memory * i, 4, 16, 4, aae);
        outFile<<std::endl;
    }

    delete are;
    delete aae;
}

void testCase4(std::ofstream &outFile) {
    int memory = 8000;
    int query_freq = 10;
    int window_size = 1 << 13;

    outFile<<"name,memory,counterNum,counterSize,thr"<<std::endl;
    for(int i = 4; i <= 16; i += 1)
        for(int counterNum = 4; counterNum <= 16; counterNum += 4) {
            outFile<<getString(memory * i, counterNum, 16, 3);
            outFile<<testTHR(window_size, memory * i, counterNum, 16, 3, 10);
            outFile<<std::endl;
        }
}

void testCase5(std::ofstream &outFile) {
    int memory = 8000;
    int query_freq = 10;
    int window_size = 1 << 13;

    outFile<<"name,memory,counterNum,counterSize,thr"<<std::endl;
    for(int i = 4; i <= 16; i += 4) {
        // Optimized
        outFile<<getString(memory * i, 4, 16, 3);
        outFile<<testTHR(window_size, memory * i, 4, 16, 3, 10);
        outFile<<std::endl;

        // Basic
        outFile<<getString(memory * i, 4, 16, 1);
        outFile<<testTHR(window_size, memory * i, 4, 16, 1, 10);
        outFile<<std::endl;
    }
}

int main(int argc,char* argv[]) {

    dataset = read_data((inputFolder + inputFile).c_str(), 1 << 20, &datasetLength);

    std::ofstream outFile(outputFolder + outputFile);

    testCase1(outFile);             // Figure 5 (a) & (b)
    // testCase2(outFile);          // Figure 5 (c) & (d)
    // testCase3(outFile);          // Figure 5 (e) & (f)
    // testCase4(outFile);          // Figure 5 (g)
    // testCase5(outFile);          // Figure 5 (h)
    
    
    outFile.close();
    return 0;
}
