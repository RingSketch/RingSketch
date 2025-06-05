#include <random>
#include <cassert>
#include <deque>
#include <iostream>
#include <fstream>
#include "Algos.h"

std::string inputFolder = "../datasets/";
std::string inputFile = "caida.dat";
std::string outputFolder = "../results/";
std::string outputFile = "freshness.csv";

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

double sum_ae[1 << 13 | 1];
std::vector<int> counter[1 << 13 | 1];

std::string test(int query_freq, int window_size, int memory, int counterNum, int counterSize, scheme* measurer, std::ofstream& outFile) {
    uint64_t c = datasetLength;

    double accu = 0;
    double num = 0;

    srand(131);
    for(int k = 0; k < 20; ++k) {
        std::deque<std::pair<int, TUPLES>> lastTime;
        RingSketch* sketch = new RingSketch(window_size, memory, counterNum, counterSize);
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

                counter[gt].push_back(sketch->countCounter(random_it->second));
                int est = sketch->estimate(random_it->second, i);

                accu += measurer->measure(est, gt);
                num += 1;
                sum_ae[gt] += measurer->measure(est, gt);
            }

            sketch->insert(tuple, i);
            lastTime.emplace_back(i, tuple);
            sketch->updateClock(1);
        }
        delete sketch;
    }

    outFile << "sum,count,avg,aae\n";
    for (int i = 1; i < window_size; i++) {
        double sum = 0;
        for (int j = 0; j < counter[i].size(); j++)
            sum += counter[i][j];
        outFile << sum << ',' << counter[i].size() << ',' << sum / counter[i].size() << ',' << sum_ae[i] / counter[i].size() << std::endl;
    }

    char returnString[200] = {0};
    sprintf(returnString, ",%.10f", accu / num);
    return std::string(returnString);
}

std::string getString(int memory, int counterNum, int counterSize, int t) {
    char returnString[200] = {0};
    sprintf(returnString, "%s,%d,%d,%d", "RingSketch", memory, counterNum, counterSize);
    return std::string(returnString);
}

int main(int argc,char* argv[]) {

    dataset = read_data((inputFolder + inputFile).c_str(), 1 << 20, &datasetLength);

    std::ofstream outFile(outputFolder + outputFile);

    scheme *are = new ARE, *aae = new AAE;

    int memory = 8000;
    int query_freq = 1;
    int window_size = 1 << 13;
    
    test(query_freq, window_size, 16 * 1024, 4, 12, are, outFile);
    
    delete are;
    delete aae;

    outFile.close();
    return 0;
}
