#include "../Abstract.h"
#include <cassert>
#include <thread>
#include <atomic>

template<typename DATA_TYPE>
class RingSketchCacheVer0Thread : public CacheAbstract<DATA_TYPE> {
public:
    RingSketchCacheVer0Thread(int cacheSize, int _counterSize, int _depth, int _bucketDepth) {
        int bucketMemory = sizeof(DATA_TYPE) + _counterSize;

        depth = _depth;
        counterSize = _counterSize;
        // bucketCount = _bucketCount;
        bucketDepth = _bucketDepth;
        bucketCount = cacheSize / _depth / _bucketDepth;

        width = depth * bucketCount * bucketDepth;

        key = new DATA_TYPE[width];
        counter = new int[width];
        memset(key, 0, sizeof(DATA_TYPE) * width);
        memset(counter, 0, sizeof(int) * width);

        updateSpeed = 16;
        lastUpdateIdx = 0;

        haveLastEvicted = false;

        updator = std::thread(RingSketchCacheVer0Thread::updateThread, 1000000, this);
    }

    ~RingSketchCacheVer0Thread() {
        delete []key;
        delete []counter;
    }

    void insert(const DATA_TYPE& x) {
        haveLastEvicted = false;

        int d = hash(x, 100) % depth;
        int delta = hash(x, 200 + d) % bucketCount * bucketDepth + d * bucketCount * bucketDepth;

        DATA_TYPE *keys = key + delta;
        int *counters = counter + delta;

        int min_i = -1;
        int min_counter = 0;
        for (int pos = 0; pos < bucketDepth; pos++) {
            if (keys[pos] == x) {
                counters[pos] = (1 << counterSize) - 1;
                return;
            }

            if (min_i == -1 || counters[pos] < min_counter) {
                min_i = pos;
                min_counter = counters[pos];
            }
        }

        lastEvicted = keys[min_i];
        haveLastEvicted = true;

        keys[min_i] = x;
        counters[min_i] = (1 << counterSize) - 1;
    }

    bool inCache(const DATA_TYPE& x) {
        haveLastEvicted = false;

        int d = hash(x, 100) % depth;
        int delta = hash(x, 200 + d) % bucketCount * bucketDepth + d * bucketCount * bucketDepth;

        DATA_TYPE *keys = key + delta;

        for (int pos = 0; pos < bucketDepth; pos++) {
            if (keys[pos] == x) {
                return true;
            }
        }

        return false;
    }

    std::vector<DATA_TYPE> getItems() {
        std::vector<DATA_TYPE> items;
        for (int i = 0; i < width; i++)
            if (counter[i] > 0)
                items.push_back(key[i]);
        return items;
    }

    void updateClock(int insertTimesPerUpdate) {
        int temp = updateSpeed * insertTimesPerUpdate;
        int j = lastUpdateIdx;
        for (int i = 0; i < temp; i++) {
            if (counter[j])
                counter[j]--;

            j = j + bucketDepth;
            if (j > width)
                j = (j - width + 1) % bucketDepth;
        }
        lastUpdateIdx = j;
    }

    static void updateThread(int scanSpeed, RingSketchCacheVer0Thread *sk);

    DATA_TYPE *key;
    int *counter;

    bool haveLastEvicted;
    DATA_TYPE lastEvicted;

    int width;
    int depth;
    int bucketCount;
    int updateSpeed;
    int bucketDepth;
    int counterSize;
    int lastUpdateIdx;

    std::thread updator;
    int insertTimesPerUpdate;
    std::atomic<int> updating;
};

template<typename DATA_TYPE>
void RingSketchCacheVer0Thread<DATA_TYPE>::updateThread(int scanSpeed, RingSketchCacheVer0Thread<DATA_TYPE> *sk) {
    while (sk->updating) {
        std::this_thread::sleep_for(std::chrono::microseconds(1000000 / scanSpeed));
        sk->updateClock(1);
    }
}

