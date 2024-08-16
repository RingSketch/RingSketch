#pragma once

#include "../Abstract.h"
#include <list>
#include <unordered_map>

template<typename DATA_TYPE>
class LRUCache : public CacheAbstract<DATA_TYPE> {
public:
    LRUCache(int capacity) : capacity(capacity) {}

    void insert(const DATA_TYPE& item) {
        this->haveLastEvicted = false;

        // Check if item already exists in cache
        auto iter = cache.find(item);
        if (iter != cache.end()) {
            // Move the item to the front of the list
            list.splice(list.begin(), list, iter->second);
            return;
        }

        // Check if cache is full
        if (cache.size() >= capacity) {
            // Remove the least recently used item from the cache
            this->haveLastEvicted = true;
            cache.erase(this->lastEvicted = list.back());
            list.pop_back();
        }

        // Add the new item to the cache and front of the list
        list.emplace_front(item);
        cache[item] = list.begin();
    }

    bool inCache(const DATA_TYPE& item) {
        return cache.find(item) != cache.end();
    }

    std::vector<DATA_TYPE> getItems() {
        std::vector<DATA_TYPE> items;
        for (auto iter = list.begin(); iter != list.end(); ++iter) {
            items.push_back(*iter);
        }
        return items;
    }

    void updateClock(int insertTimesPerUpdate) {
        return;
    }

private:

    int capacity;
    std::list<DATA_TYPE> list;
    std::unordered_map<DATA_TYPE, typename std::list<DATA_TYPE>::iterator> cache;
};

