#ifndef _GSU_H_
#define _GSU_H_

#include "../../algos/Util.h"

#define CELLNUM 8

typedef std::unordered_map<ItemPair, COUNT_TYPE> HashMap;

typedef TUPLES DATA_TYPE;

template <uint32_t HASH_NUM>
class GSU{
public:

    struct Bucket{
        struct Cell{
            DATA_TYPE item;
            uint32_t interval;
            COUNT_TYPE count;
        };

        Cell cells[CELLNUM];
        COUNT_TYPE fail;
    };

    GSU(uint32_t _MEMORY){
        LENGTH = _MEMORY / sizeof(Bucket);

        buckets = new Bucket[LENGTH];
        memset(buckets, 0, sizeof(Bucket) * LENGTH);
    }

    ~GSU(){
        delete [] buckets;
    }

    void Insert(const ItemPair& temp){
        uint32_t position = hash(temp, HASH_NUM) % LENGTH, min_pos = 0;
        COUNT_TYPE min_count = INT32_MAX;

        for(uint32_t j = 0;j < CELLNUM;++j){
            if(buckets[position].cells[j].item == temp.item &&
                    buckets[position].cells[j].interval == temp.time){
                buckets[position].cells[j].count += 1;
                return;
            }

            if(buckets[position].cells[j].count < min_count){
                min_count = buckets[position].cells[j].count;
                min_pos = j;
            }
        }

        if(rng() % (2 * min_count - buckets[position].fail + 1) == 0){
            buckets[position].cells[min_pos].item = temp.item;
            buckets[position].cells[min_pos].interval = temp.time;
            if(min_count == 0)
                buckets[position].cells[min_pos].count = 1;
            else
                buckets[position].cells[min_pos].count += buckets[position].fail / min_count;
            buckets[position].fail = 0;
        }
        else{
            buckets[position].fail += 1;
        }
    }

    HashMap Report(COUNT_TYPE HIT){
        HashMap ret;

        for(uint32_t i = 0;i < LENGTH;++i){
            for(uint32_t j = 0;j < CELLNUM;++j){
                if(buckets[i].cells[j].count > HIT){
                    ret[ItemPair(buckets[i].cells[j].interval,
                            buckets[i].cells[j].item)]
                    = buckets[i].cells[j].count;
                }
            }
        }

        return ret;
    }

private:
    uint32_t LENGTH;

    Bucket* buckets;
};

#endif
