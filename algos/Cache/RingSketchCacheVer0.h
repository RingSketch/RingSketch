#include "../Abstract.h"
#include <cassert>

template<typename DATA_TYPE>
class RingSketchCacheVer0 : public CacheAbstract<DATA_TYPE> {
public:
    RingSketchCacheVer0(int cacheSize, int _counterSize, int _depth, int _bucketDepth) {
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

        this->haveLastEvicted = false;
    }

    ~RingSketchCacheVer0() {
        delete []key;
        delete []counter;
    }

    void insert(const DATA_TYPE& x) {
        this->haveLastEvicted = false;

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

        this->lastEvicted = keys[min_i];
        this->haveLastEvicted = true;

        keys[min_i] = x;
        counters[min_i] = (1 << counterSize) - 1;
    }

    bool inCache(const DATA_TYPE& x) {
        this->haveLastEvicted = false;

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

    DATA_TYPE *key;
    int *counter;

    int width;
    int depth;
    int bucketCount;
    int updateSpeed;
    int bucketDepth;
    int counterSize;
    int lastUpdateIdx;
};

