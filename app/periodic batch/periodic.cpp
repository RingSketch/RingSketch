#include <random>
#include <cassert>
#include <deque>
#include <map>
#include <set>
#include <unordered_set>
#include "../../algos/Algos.h"
#include "GSU.h"

uint64_t datasetLength;
std::pair<TUPLES*, TIMESTAMP*> dataset;

Abstract* chooseSketch(int sketchtype, int window_size, int memory, int counterNum, int counterSize) {
    switch (sketchtype) {
    case 0:
        return new RingSketch(window_size, memory, counterNum, counterSize);
    case 1:
        return new TimeoutBloomFilter(memory, counterNum);
    }
    abort();
}

void CheckError(HashMap mp, HashMap temp, uint32_t HIT){
    double real = 0, estimate = 0, both = 0;
    double aae = 0, are = 0, cr = 0, pr = 0, f1 = 0;

    double hot = 0;

    estimate = temp.size();

    for(auto it = mp.begin();it != mp.end();++it){
        if(it->second > HIT){
            real += 1;
            if(temp.find(it->first) != temp.end()){
                both += 1;
                COUNT_TYPE value = temp[it->first];
                aae += abs(it->second - value);
                are += abs(it->second - value) / (double)it->second;
            }
        }
    }

    if(both <= 0){
        std::cout << "Not Real" << std::endl;
    }
    else{
        aae /= both;
        are /= both;
    }

    cr = both / real;

    if(estimate <= 0){
        std::cout << "Not Find" << std::endl;
    }
    else{
        pr = both / estimate;
    }

    printf("%.10f,%.10f,%.10f,%.10f\n", aae, are, cr, pr);
}


std::string folder = "../../dataset/";

std::string file[1] = {"zipf_1.0.dat"};

int main(int argc,char* argv[]) {
    srand(233);

    dataset = read_data_zipf((folder + file[0]).c_str(), 1 << 22, &datasetLength);

    freopen("result_periodic.csv", "w", stdout);
    std::cout << "name,memory,aae,are,rr,pr\n";
    for (int j = 0; j < 2; j++)
        for (int mem = 1; mem <= 5; mem ++) {
            std::unordered_map<TUPLES, int32_t> lastTime;
            std::unordered_map<ItemPair, int32_t> cnt;

            int memory = (mem * 25 + 25) * 1024;

            GSU<2> gsu(memory * 0.85);
            Abstract *sketch = chooseSketch(j, 1 << 14, memory * 0.15, 6, 8);

            std::cout << (j == 0 ? "RingSketch" : "PeriodicSketch") << ',';
            std::cout << memory << ',';

            double are = 0, c = 0;

            int lastTimestamp = 0;
            double startTime = dataset.second[0];
            for (int i = 0; i < datasetLength; i++) {
                TUPLES tuple = dataset.first[i];
                int timestamp = i;

                if (lastTime.count(tuple)) {
                    int realDelta = timestamp - lastTime[tuple];
                    if (realDelta < (1 << 14)) {
                        realDelta /= 1000;
                        ItemPair pair1(realDelta, tuple);
                        cnt[pair1]++;
                        int estDelta = sketch->estimate(tuple, timestamp) / 1000;

                        are += fabs(realDelta - estDelta) / realDelta;
                        c += 1;

                        ItemPair pair2(estDelta, tuple);
                        gsu.Insert(pair2);
                    }
                }
                lastTime[tuple] = timestamp;

                sketch->insert(tuple, timestamp);
                sketch->updateClock(1);
            }

            int threshold = datasetLength * 0.0001;

            HashMap r = gsu.Report(threshold);

            CheckError(cnt, r, threshold);
        }

    return 0;
}
