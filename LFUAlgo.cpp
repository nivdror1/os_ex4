#include "LFUAlgo.h"
#include <algorithm>
#include <zconf.h>
#include <cstring>
#include <sys/time.h>
#include <iostream>


/**
 * c-tor
 * @param blocks_num the number of blocks in the cache* get the map of the cache buffer
 * @param size the block size
 **/
LFUAlgo::LFUAlgo(int blocks_num, size_t size): CacheAlgorithm(blocks_num,size){}

LFUAlgo::~LFUAlgo(){
    for(auto iter= cacheBuffer.begin();iter!= cacheBuffer.end();iter++){
        delete (*iter).second;
    }
}

/**
 * find the minimum block that is saved in the cache in order to remove it
 * @return a pair that consist of the fd and the block number
 */
void LFUAlgo::eraseMinimum(){
    //search for the minimum block
    auto minBlock = std::min_element(cacheBuffer.begin(),cacheBuffer.end(),CompareItemsByFrequentlyUsed());
    if(minBlock!=cacheBuffer.end()){
        //delete it
        delete minBlock->second;
        cacheBuffer.erase(minBlock);
    }
}

void LFUAlgo::updateCacheAfterHit(BLOCK_ID currentBlockId){
    auto searchedBlock = cacheBuffer.find(currentBlockId);
    (*searchedBlock).second->incrementCount();
}

void LFUAlgo::insertNewBlockToCache(BLOCK_ID currentBlockId,Block* block){
    cacheBuffer.insert(std::pair<BLOCK_ID,Block*> (currentBlockId,block));
}

/**
 * sort the cache into a vector of block
 * @return a vector of all the blocks in the cache
 */
std::list<BLOCK_ID> LFUAlgo::getOrderedCache(){
    std::list<BLOCK_ID> orderedCache;
    auto compare = [this](const BLOCK_ID& left , const BLOCK_ID& right){
        return cacheBuffer.at(left)->getCount() < cacheBuffer.at(right)->getCount();
    };
    for(auto curItem: cacheBuffer){
        orderedCache.push_back(curItem.first);
    }
    orderedCache.sort(compare);
    return orderedCache;
}


