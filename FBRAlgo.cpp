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
    _sizeOfNewSection = (int)(blocks_num*f_new);
    _sizeOfOldSection = (int)(blocks_num*f_old);

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

    if(cacheBuffer.size()>=numberOfBlocks - _sizeOfOldSection) {
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
/**
 * when there is hit, read a block or a part of it from the cache
 * and relocated the block id to the end of list
 * @param currentBlockId the file descrioptor and the block number
 * @param count count how many bytes to be read
 * @param currentBlockBuffer the current buffer
 * @param block a block object
 * @param offset offset the offset to begin reading
 * @return the number of bytes read
 */
int FBRAlgo::hitCache(BLOCK_ID currentBlockId, size_t count,void* currentBlockBuffer ,Block * block, off_t offset) {
    //move the block id to end of the list

    auto searchedBlockId = std::find(orderedCache.begin(),
                                     orderedCache.end(), currentBlockId);
    orderedCache.erase(searchedBlockId);
    orderedCache.push_back(currentBlockId);
//    std::cout<<"this is the block id:"<< currentBlockId.first<<" "<<currentBlockId.second<<std::endl;
//    for(auto curItem: orderedCache){
//        std::cout<<curItem.first<<"  "<<curItem.second<<std::endl;
//    }
//    std::cout<<std::endl;
    // todo should a block need to know that state issue? since it's only use for FBR
    if (block->getState() != State::New){
        block->incrementCount();
    }

    incrementNumberOfHits(); //increment the hit number
    //read from the cache
    return block->getPartOfBlockContent(currentBlockBuffer, offset, count);
}

/**
 * when there is a miss, if the cache is full erase the minimum.
 * read the block from the disk into the cache, and fill the current
 * @param currentBlockId the file descriptor and the block number
 * @param count count how many bytes to be read
 * @param block block a block object
 * @param offset offset the offset to begin reading
 * @param fileInfo a stat object represented the file info
 * @return the number of bytes read
 */
int FBRAlgo::missCache(BLOCK_ID currentBlockId ,size_t count ,Block* block,
                       off_t offset,void *currentBlockBuffer ){


    if(cacheBuffer.size()==getNumberOfBlocks()){
        eraseMinimum();
    }
    changeState();

    incrementNumberOfMisses();

    if(pread(currentBlockId.first,currentBlockBuffer,this->blockSize,offset-(offset% blockSize))==-1){
        return -1;
    }

    try {
        block = new Block(currentBlockBuffer,blockSize, currentBlockId.second,currentBlockId.first);
    }catch (std::bad_alloc e){
        //todo error maybe throw exception
    }
    cacheBuffer.insert(std::make_pair(currentBlockId,block));
    orderedCache.push_back(currentBlockId);

//    std::cout<<"this is the block id:"<< currentBlockId.first<<" "<<currentBlockId.second<<std::endl;
//    for(auto curItem: orderedCache){
//        std::cout<<curItem.first<<"  "<<curItem.second<<std::endl;
//    }
//    std::cout<<std::endl;

    return block->getPartOfBlockContent(currentBlockBuffer,offset,count);
}

/**
 * search for the block in the cache, if the block is in the cache read from it
 * else, remove a block from the cache, and read the block from the disk
 * @param fd the file descriptor
 * @param currentBlockNumber the current block to be read
 * @param currentBlockBuffer the current buffer
 * @param offset the offset to begin reading
 * @param count how many bytes to be read
 * @param fileInfo a stat object reperesented the file info
 * @return the number of bytes read
 */
int FBRAlgo::read(int fd,int currentBlockNumber, void* currentBlockBuffer,size_t count, off_t
offset){

    BLOCK_ID currentBlockId =std::make_pair(fd,currentBlockNumber);
    Block * block= getBlockFromCache(fd,currentBlockNumber);
    if(block!= nullptr){
        return hitCache(currentBlockId, count,currentBlockBuffer , block, offset);
    }
    return missCache(currentBlockId ,count ,block,
                     offset,currentBlockBuffer );

}

/**
 * sort the cache into a vector of block
 * @return a vector of all the blocks in the cache
 */
std::list<BLOCK_ID> FBRAlgo::getOrderedCache(){
    return orderedCache;
}
