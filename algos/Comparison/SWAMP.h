#ifndef __SWAMP_H__
#define __SWAMP_H__

#include "../Abstract.h"
#include "../CuckooMap.h"

#include <iostream>
#include <cstring>
#include <algorithm>
#include <assert.h>
#include <cmath>

#define L 32

class SWAMP: public Abstract{
    typedef uint32_t KEY_TYPE;
    typedef std::pair<int, int> VALUE_TYPE;
    int window_size;
    KEY_TYPE *fingerprints;
    int curr;
    int Z;
    CuckooMap<KEY_TYPE, VALUE_TYPE> tinyTable;
    uint32_t fp_seed;

    void updateCD(int prevFreq, int xFreq);
    void insertFingerprint(uint32_t fingerprint, int t);

public:
    SWAMP(int _window, int _memory);
    ~SWAMP();
    void insert(TUPLES x, int t);
    void updateClock(int insertTimesPerUpdate = 1);
    int estimate(TUPLES x, int t);
    void updateRange(int beg, int end, int val);
    double cardinalEst(int threshold);
};

SWAMP::SWAMP(int _window, int _memory):
    window_size(_window), curr(0), Z(0), fp_seed(233),
    tinyTable(CuckooMap<KEY_TYPE, VALUE_TYPE>::Memory2Size(_memory - _window * 4))
{
    this->name = "SWAMP";
    
    fingerprints = new KEY_TYPE[_window];
    
    memset(fingerprints, 0, sizeof(KEY_TYPE) * _window);
}

SWAMP::~SWAMP()
{
    delete []fingerprints;
}

void SWAMP::insertFingerprint(KEY_TYPE fingerprint, int t)
{
    if(tinyTable.Lookup(fingerprint)) {
        VALUE_TYPE x = tinyTable[fingerprint];
        tinyTable.Replace(fingerprint, std::make_pair(x.first + 1, t));
    }
    else {
        tinyTable.Insert(fingerprint, std::make_pair(1, t));
    }
}

void SWAMP::insert(TUPLES x, int t)
{
    bool prevExi = tinyTable.Lookup(fingerprints[curr]);

    if(!prevExi) {
        fingerprints[curr] = hash(x, fp_seed);
        insertFingerprint(fingerprints[curr], t);

        updateCD(0, tinyTable[fingerprints[curr]].first);
    }
    else {
        VALUE_TYPE prev = tinyTable[fingerprints[curr]];

        if(prev.first > 1)
            tinyTable.Replace(fingerprints[curr], std::make_pair(prev.first - 1, prev.second));
        else if(prev.first == 1)
            tinyTable.Delete(fingerprints[curr]);

        fingerprints[curr] = hash(x, fp_seed);
        insertFingerprint(fingerprints[curr], t);

        updateCD(prev.first, tinyTable[fingerprints[curr]].first);
    }

    curr = (curr + 1) % window_size;
}

void SWAMP::updateCD(int prevFreq, int xFreq)
{
    if(prevFreq == 1)
        Z--;

    if(xFreq == 1)
        Z++;
}

void SWAMP::updateClock(int insertTimesPerUpdate)
{
    return;
}

int SWAMP::estimate(TUPLES x, int t)
{
    uint32_t fingerprint = hash(x, fp_seed);
    if(tinyTable.Lookup(fingerprint)) {
        return t - tinyTable[fingerprint].second;
    }
    
    return window_size;
}

void SWAMP::updateRange(int beg, int end, int val)
{
    return;
}

double SWAMP::cardinalEst(int threshold)
{
    double tmp = 2ll << L;
    return log(1 - Z / tmp) / log(1 - 1 / tmp);
}

#endif