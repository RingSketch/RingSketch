#ifndef _HyperBloomFilter_H_
#define _HyperBloomFilter_H_

#include "../Abstract.h"

#include <iostream>
#include <cstring>
#include <algorithm>

typedef unsigned char Counter_t;

class HyperBloomFilter: public Abstract{
    int window_sz;
    int batch_window;
    int countersize;
    int depth;
    int width;
    int group_num;
    int group_length;
    int updateLen;

    Counter_t **arrays;

    bool timeout(int sNow, int sCell);
    int subtract(int sNow, int sCell);

public:
    HyperBloomFilter(int _window, int _memory, int _depth, int _group_length, int _countersize=2);
    ~HyperBloomFilter();
    void insert(TUPLES x, int t);
    void updateClock(int insertTimesPerUpdate=1);
    int estimate(TUPLES x, int);
    void updateRange(int beg, int end, int val);
    double cardinalEst(int threshold, int t);
};

// unit of memory is byte
HyperBloomFilter::HyperBloomFilter(int _window, int _memory, int _group_length, int _depth, int _countersize)
    : window_sz(_window), depth(_depth), countersize(_countersize), group_length(_group_length)
{
    this->name = "HyperBF";

    batch_window = window_sz / ((1 << _countersize) - 2);

    group_num = 8 * _memory / _countersize / _depth / _group_length;

    width = group_length * group_num;

    arrays = new Counter_t*[depth];
    for(int i = 0; i < depth; ++i){
        arrays[i] = new Counter_t[width];
        memset(arrays[i], 0, sizeof(arrays[i]));
    }
}

HyperBloomFilter::~HyperBloomFilter()
{
    for(int i = 0; i < depth; ++i) delete[] arrays[i];
    delete[] arrays;
}

void HyperBloomFilter::insert(TUPLES x, int t)
{
    int sNow = (t % window_sz) / batch_window + 1;

    for(int i = 0; i < depth; ++i){
        int pos = hash(x, 200 + i) % width;
        int group = pos / group_length;

        for(int j = group * group_length; j < (group + 1) * group_length; ++j){
            if(arrays[i][j] != 0 && timeout(sNow, arrays[i][j]))
                arrays[i][j] = 0;
        }
        arrays[i][pos] = sNow;
    }
}

void HyperBloomFilter::updateClock(int insertTimesPerUpdate)
{
    return;
}

int HyperBloomFilter::estimate(TUPLES x, int t)
{
    int sNow = (t % window_sz) / batch_window + 1;
    int m = 0;
    
    for(int i = 0; i < depth; ++i){
        int pos = hash(x, 200 + i) % width;
        int timeSpan = subtract(sNow, arrays[i][pos]);
        m = m>=timeSpan?m:timeSpan;
    }
    
    return (2ll * m + 1) * batch_window / 2;
}

void HyperBloomFilter::updateRange(int beg, int end, int val)
{
    return;
}

bool HyperBloomFilter::timeout(int sNow, int sCell){
    if(sNow == (Counter_t) 0xFF)
        return sCell == 1;
    else
        return sNow == sCell - 1;
}

int HyperBloomFilter::subtract(int sNow, int sCell){
    if(sCell <= sNow) 
        return sNow - sCell;
    else
        return ((Counter_t) 0xFF) - 1 - (sCell - sNow);
}

double HyperBloomFilter::cardinalEst(int threshold, int t) {
    int sNow = (t % window_sz) / batch_window + 1;
    int m = 0;

    double u = 0;
    for (int j = 0; j < depth; j++)
        for(int i = 0; i < width; ++i){
            int m = subtract(sNow, arrays[j][i]);
            long long delta = (2ll * m + 1) * batch_window / 2;
            if (delta > threshold)
                u += 1;
        }
    return -width * depth * log(u / width / depth) / depth;
}

#endif //_HyperBloomFilter_H_
