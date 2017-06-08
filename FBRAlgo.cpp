//
// Created by ido.shachar on 6/5/17.
//

#include <algorithm>
#include <zconf.h>
#include <iostream>
#include "FBRAlgo.h"

/**
 * c-tor
 * @param blocks_num the number of blocks in the cache* get the map of the cache buffer
 * @param size the block size
 **/
FBRAlgo::FBRAlgo(int blocks_num,size_t size, double f_old , double f_new ):
        CacheAlgorithm(blocks_num,size){
    _sizeOfNewSection = (unsigned int)(blocks_num*f_new);
    _sizeOfOldSection = (unsigned int)(blocks_num*f_old);

//    //define an iterator for the node of the end of the old section
//    auto endOfOldSection = orderedCache.begin();
//    std::advance(endOfOldSection, _sizeOfOldSection);
//    _endOfOldSection = endOfOldSection;
//
//    //define an iterator for the node of the end of the middle section
//    int middleSectionSize = numberOfBlocks - _sizeOfNewSection - _sizeOfOldSection;
//    if (middleSectionSize > 0){
//        _hasMiddle =true;
//        _endOfMiddleSection = orderedCache.begin();
//        std::advance(_endOfMiddleSection, _sizeOfOldSection + middleSectionSize);
//    }
}

FBRAlgo::~FBRAlgo(){
    for(auto iter= cacheBuffer.begin();iter!= cacheBuffer.end();iter++){
        delete (*iter).second;
    }
}

void FBRAlgo::updateLastOldSectionItem(int remainingOldSectionSize){

    if(cacheBuffer.size() >= numberOfBlocks - _sizeOfOldSection) {
        //define an iterator for the node of the end of the old section
        auto endOfOldSection = orderedCache.begin();
        if(remainingOldSectionSize > 0){
            std::advance(endOfOldSection, remainingOldSectionSize);
        }
        _endOfOldSection = endOfOldSection;
        cacheBuffer.at(*_endOfOldSection)->setState(State::Old);
    }

}

void FBRAlgo::updateLastMiddleSectionItem(int remainingOldSectionSize, int middleSectionSize, int remainingMiddleSectionSize){
    //define an iterator for the node of the end of the middle section

    if (middleSectionSize > 0 && cacheBuffer.size() >= _sizeOfNewSection){
        _endOfMiddleSection = orderedCache.begin();
        if(remainingMiddleSectionSize > 0){
            std::advance(_endOfMiddleSection, remainingOldSectionSize + middleSectionSize);
        }

        // change last element in middle section state to middle
        cacheBuffer.at(*_endOfMiddleSection)->setState(State::Middle);
    }
}

void FBRAlgo::changeState() {
    int middleSectionSize = numberOfBlocks - _sizeOfNewSection - _sizeOfOldSection;
    int remainingOldSectionSize = (int)cacheBuffer.size() -_sizeOfNewSection  - middleSectionSize;
    int remainingMiddleSectionSize = (int)cacheBuffer.size() -_sizeOfNewSection- remainingOldSectionSize;

    updateLastOldSectionItem(remainingOldSectionSize);
    updateLastMiddleSectionItem(remainingOldSectionSize, middleSectionSize, remainingMiddleSectionSize);
}

/**
 * find the minimum block that is saved in the cache in order to remove it
 * @return a pair that consist of the fd and the block number
 */
void FBRAlgo::eraseMinimum(){


    // todo ugly code, but with extern struct you cannot use cacheBuffer and it's critical
    auto compare = [this](const BLOCK_ID& left , const BLOCK_ID& right){
        return cacheBuffer.at(left)->getCount() < cacheBuffer.at(right)->getCount();
    };
    auto minBlock = std::min_element(orderedCache.begin(), _endOfOldSection, compare);


    //search and delete it from the cache map
    auto searchedBlock = cacheBuffer.find(*minBlock);
    delete searchedBlock->second;
    cacheBuffer.erase(searchedBlock);
    //remove from the ordered cache list
    orderedCache.erase(minBlock);
}

void FBRAlgo::updateCacheAfterHit(BLOCK_ID currentBlockId){
    //move the block id to end of the list

    auto searchedBlockId = std::find(orderedCache.begin(),
                                     orderedCache.end(), currentBlockId);
    Block* currentBlock = cacheBuffer.at(*searchedBlockId);
    if (currentBlock->getState() != State::New){
        currentBlock->incrementCount();
    }
    orderedCache.erase(searchedBlockId);
    orderedCache.push_back(currentBlockId);
}

void FBRAlgo::insertNewBlockToCache(BLOCK_ID currentBlockId,Block* block){
    cacheBuffer.insert(std::pair<BLOCK_ID,Block*> (currentBlockId,block));
    orderedCache.push_back(currentBlockId);
}


/**
 * search the cache for the block, if found return the block
 * @param fd the file descriptor
 * @param currentBlockNumber the current block to be read
 * @return upon success return the block , else return nullptr
 */
Block* FBRAlgo::getBlockFromCache(int fd, int currentBlockNumber) const{
    auto searchedBlock = cacheBuffer.find(std::make_pair(fd,currentBlockNumber));
    if(searchedBlock!=cacheBuffer.end()){
        return searchedBlock->second;
    }
    return nullptr;

}

void FBRAlgo::updateCacheAfterMiss(){
	changeState();
    if(cacheBuffer.size()==getNumberOfBlocks()){
        eraseMinimum();
    }

}


/**
 * sort the cache into a vector of block
 * @return a vector of all the blocks in the cache
 */
std::list<BLOCK_ID> FBRAlgo::getOrderedCache(){
    return orderedCache;
}
