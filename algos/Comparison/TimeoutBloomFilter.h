#ifndef _TimeoutBloomFilter_H_
#define _TimeoutBloomFilter_H_

#include "../Abstract.h"

#include <iostream>
#include <cstring>
#include <algorithm>

class TimeoutBloomFilter: public Abstract {
    int width;
    int hash_num;

    int* buckets;

public:
    TimeoutBloomFilter(int _memory, int _hashnum);
    ~TimeoutBloomFilter();
    void insert(TUPLES x, int t);
    int estimate(TUPLES x, int t);
    void updateClock(int);
    void updateRange(int, int, int);
    double cardinalEst(int threshold, int t);
};


TimeoutBloomFilter::TimeoutBloomFilter(int _memory, int _hash_num)
    : hash_num(_hash_num)
{
    width = _memory / 8;

    this->name = "TOBF";

    buckets = new int[width];
    memset(buckets, 0, sizeof(int) * width);
}

TimeoutBloomFilter::~TimeoutBloomFilter()
{
    delete[] buckets;
}

void TimeoutBloomFilter::insert(TUPLES x, int t)
{
    for(int i = 0; i < hash_num; ++i)
    {
        int pos = hash(x, 200 + i) % width;
        buckets[pos] = t;
    }
}

int TimeoutBloomFilter::estimate(TUPLES x, int t)
{
    int minimum = t;
    for(int i = 0; i < hash_num; ++i)
    {
        int pos = hash(x, 200 + i) % width;
        if(buckets[pos]<minimum) minimum = buckets[pos];
    }
    return t - minimum;
}

void TimeoutBloomFilter::updateClock(int) {
    return;
}

void TimeoutBloomFilter::updateRange(int, int, int) {
    return;
}

double TimeoutBloomFilter::cardinalEst(int threshold, int t) {
    double u = 0;
    for(int i = 0; i < width; ++i)
        u += (buckets[i] < (t - threshold)) ? 1 : 0;
    u = std::max(u, 1.);
    return 1. * -width * log(u / width) / hash_num;
}

#endif //_TimeoutBloomFilter_H_
