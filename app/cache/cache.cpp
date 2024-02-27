#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>

#include "../../algos/Algos.h"

#include "treap.h"

CacheAbstract<TUPLES> *chooseCache(int type, int capacity, int counterSize, int depth, int bucketDepth) {
    switch (type) {
    case 0:
        return new LRUCache<TUPLES>(capacity);
    case 1:
        return new RingSketchCacheVer0<TUPLES>(capacity, counterSize, depth, bucketDepth);
    default:
        abort();
        break;
    }
}

std::pair<TUPLES*, TIMESTAMP*> datasetPair;
uint64_t c;

void testSimHit() {
    for (int d = 4; d <= 32; d <<= 1)
        for (int j = 2; j < 9; j++) {
            int capacity = j * 1000;

            std::cout << capacity << ',';
            if (d == 4)
                std::cout << "Ideal,";
            else
                std::cout << d << ',';

            CacheAbstract<TUPLES> *cache = chooseCache(d != 4, capacity, 12, 4, d);

            int hitCounter = 0;
            Node *treapRoot = NULL;
            int simCount = 0;
            double simSum = 0;
            std::unordered_map<TUPLES, double> lastTime;
            for (int i = 0; i < c; i++) {
                if (cache->inCache(datasetPair.first[i]))
                    hitCounter++;
                cache->insert(datasetPair.first[i]);
                if (cache->haveLastEvicted && i) {
                    TUPLES evi = cache->lastEvicted;
                    simSum += 1. * (rank(treapRoot, lastTime[evi]) - 1) / treapRoot->size;
                    simCount += 1;
                    treapRoot = erase(treapRoot, lastTime[evi]);
                }
                cache->updateClock(1);
                if (lastTime.find(datasetPair.first[i]) != lastTime.end())
                    treapRoot = erase(treapRoot, lastTime[datasetPair.first[i]]);
                lastTime[datasetPair.first[i]] = i;
                treapRoot = insert(treapRoot, i);
            }

            std::cout << 1 - simSum / simCount << ',';
            std::cout << 1. * hitCounter / c << '\n';
        }
}

void testThroughput() {
    for (int d = 8; d <= 32; d <<= 1)
        for (int j = 2; j < 9; j++) {
            int capacity = j * 1000;

            std::cout << capacity << ',' << d << ',';

            RingSketchCacheVer0Thread<TUPLES> *cache = new RingSketchCacheVer0Thread<TUPLES>(capacity, 12, 4, d);

            int hitCounter = 0;
            Node *treapRoot = NULL;
            int simCount = 0;
            double simSum = 0;
            std::unordered_map<TUPLES, double> lastTime;

            auto t1 = std::chrono::steady_clock::now();
            for (int i = 0; i < c; i++)
                cache->insert(datasetPair.first[i]);

            auto t2 = std::chrono::steady_clock::now();
            auto t3 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            printf("%.10f\n", c / (1.0 * t3));
        }
}

std::string folder = "../../dataset/";

std::string file[1] = {"zipf_1.0.dat"};

int main(int argc,char* argv[]) {
    datasetPair = read_data_zipf((folder + file[0]).c_str(), 1 << 20, &c);

    freopen("./result_sim_hit.csv", "w", stdout);
    std::cout << "cacheSize,bucketDepth,similarity,hitRate\n";
    testSimHit();

    freopen("./result_throughput.csv", "w", stdout);
    std::cout << "cacheSize,bucketDepth,throughput\n";
    testThroughput();
    return 0;
}
