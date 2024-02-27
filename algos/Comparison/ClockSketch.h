#ifndef _ClockSketch_H_
#define _ClockSketch_H_

#include "../Abstract.h"

#include <iostream>
#include <cstring>
#include <algorithm>

class ClockSketch: public Abstract{
    int width;
    int hash_num;
    int updateLen;

    short *clocks;

    int lastUpdateIdx;
    int clocksize;
public:
    ClockSketch(int _window, int _memory, int _hash_num, int _countersize=2);
    ~ClockSketch();
    void insert(TUPLES x, int);
    void updateClock(int insertTimesPerUpdate=1);
    int estimate(TUPLES x, int);
    void updateRange(int beg, int end, int val);
    double cardinalEst(int threshold, int t);
};

ClockSketch::ClockSketch(int _window, int _memory, int _hash_num, int _countersize)
    : hash_num(_hash_num), lastUpdateIdx(0), clocksize(_countersize)
{
    this->name = "ClockSketch";

    width = 8 * _memory / _countersize;
    updateLen = ((1ll << _countersize) - 2) * width / _window;

    clocks = new short[width];
    memset(clocks, 0, sizeof(short) * width);
}

ClockSketch::~ClockSketch()
{
    delete[] clocks;
}

void ClockSketch::insert(TUPLES x, int)
{
    for(int i = 0; i < hash_num; ++i)
    {
        int pos = hash(x, 100 + i) % width;
        clocks[pos] = (1 << clocksize) - 1;
    }
}

void ClockSketch::updateClock(int insertTimesPerUpdate)
{
    int temp = updateLen * insertTimesPerUpdate;
    int subAll = temp / width;
    int len = temp % width;
    updateRange(0, width, subAll);

    int beg, end;
    if (lastUpdateIdx + len >= width) {
        beg = lastUpdateIdx, end = (lastUpdateIdx + len) % width;
        updateRange(beg, width, 1);
        updateRange(0, end, 1);
    }else {
        beg = lastUpdateIdx, end = lastUpdateIdx + len;
        updateRange(beg, end, 1);
    }
    lastUpdateIdx = end;
}

int ClockSketch::estimate(TUPLES x, int)
{
    int mini = (1 << clocksize) - 1;
    for(int i = 0; i < hash_num; ++i)
    {
        int pos = hash(x, 100 + i) % width;
        if(clocks[pos] < mini) mini = clocks[pos];
    }
    return (((long long)width) * ((1 << clocksize) - 1 - mini)) / updateLen;
}

void ClockSketch::updateRange(int beg, int end, int val)
{
    if(val <= 0)    return;
    
    __m256i _subVal = _mm256_set1_epi16(short(val));
    while(beg + 16 <= end)
    {
        __m256i clock = _mm256_loadu_si256((__m256i*)&clocks[beg]);
        __m256i subRes = _mm256_subs_epu16(clock, _subVal);
        _mm256_storeu_si256((__m256i*)&clocks[beg], subRes);
		beg += 16;
    }

    while(beg < end){
        if(clocks[beg] <= val)
            clocks[beg] = 0;
        else
            clocks[beg] -= val;
        beg++;
    }
}

double ClockSketch::cardinalEst(int threshold, int t) {
    int k = (1ll * threshold * updateLen + width - 1) / width;

    double u = 0;
    for(int i = 0; i < width; ++i) {
        long long tmp = (1 << clocksize) - 2 - clocks[i];
        if (tmp * width / updateLen > threshold)
            u++;
    }
    return 1. * -width * log(u / width) / hash_num;
}


#endif //_ClockSketch_H_
