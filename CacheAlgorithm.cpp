//
// Created by ido.shachar on 5/29/17.
//

#include <zconf.h>
#include <iostream>
#include "CacheAlgorithm.h"


/**
 * c-tor
 * @param blocks_num the number of blocks in the cache
 * @param blockSize the block size
 */
CacheAlgorithm::CacheAlgorithm(int blocks_num,size_t size):numberOfHits(0), numberOfMisses(0),
                                                           numberOfBlocks((unsigned int) blocks_num), blockSize(size) {}

/**
 * get the number of hits
 * @return return the number of hits
 */
int CacheAlgorithm::getNumberOfHits(){
    return this->numberOfHits;
}

/**
 * get the number of misses
 * @return return the number of misses
 */
int CacheAlgorithm::getNumberOfMisses() {
    return this->numberOfMisses;
}

/**
 * increment the number of hits
 */
void CacheAlgorithm::incrementNumberOfHits(){
    this->numberOfHits++;
}

/**
 * increment the number of misses
 */
void CacheAlgorithm::incrementNumberOfMisses(){
    this->numberOfMisses++;
}

/**
 * get the number of blocks in the cache
 * @return return the number of blocks in the cache
 */
unsigned int CacheAlgorithm::getNumberOfBlocks(){
    return this->numberOfBlocks;
}

/**
 * when there is hit, read a block or a part of it from the cache
 * and relocated the block id to the end of list
 * @param currentBlockId the file descriptor and the block number
 * @param count count how many bytes to be read
 * @param currentBlockBuffer the current buffer
 * @param block a block object
 * @param offset offset the offset to begin reading
 * @return the number of bytes read
 */
int CacheAlgorithm::hitCache(BLOCK_ID currentBlockId, size_t count,void* currentBlockBuffer ,
                             Block * block, off_t offset) {
    updateCacheAfterHit(currentBlockId);

    incrementNumberOfHits(); //increment the hit number
    //read from the cache
    return block->getPartOfBlockContent(currentBlockBuffer, offset, count);
}

/**
 * Checks if the cache if full, if so remove the minimum element from it.
 */
void CacheAlgorithm::updateCacheAfterMiss(){
    if(cacheBuffer.size()==getNumberOfBlocks()){
        eraseMinimum();
    }
}


/**
 * when there is a miss, if the cache is full erase the minimum element in such case.
 * Read the block from the disk into the cache, and fill the current
 * @param currentBlockId the file descriptor and the block number
 * @param count count how many bytes to be read
 * @param absPath the absolute path of the file
 * @param offset offset the offset to begin reading
 * @param currentBlockBuffer a buffer to read to.
 * @return the number of bytes that actually have been read
 */
int CacheAlgorithm::missCache(BLOCK_ID currentBlockId ,size_t count ,char* absPath,
                              off_t offset,void *currentBlockBuffer ){

    Block* block;
    updateCacheAfterMiss();

    incrementNumberOfMisses();

    if(pread(currentBlockId.first,currentBlockBuffer,this->blockSize,offset-(offset% blockSize))==-1){
        return -1;
    }

    try {
        block = new Block(currentBlockBuffer,blockSize, currentBlockId.second,currentBlockId
                .first, absPath);
    }catch (std::bad_alloc e){
        return -1;
    }

    insertNewBlockToCache(currentBlockId,block);

    return block->getPartOfBlockContent(currentBlockBuffer,offset,count);
}

/**
 * Scans the cache to find the block that match to the given file descriptor and block number.
 * @param fd the file descriptor
 * @param currentBlockNumber the current block to be read
 * @return upon success return the block , else return nullptr
 */
Block* CacheAlgorithm::getBlockFromCache(int fd, int currentBlockNumber) const{
    auto searchedBlock = cacheBuffer.find(std::make_pair(fd,currentBlockNumber));
    if(searchedBlock!=cacheBuffer.end()){
        return searchedBlock->second;
    }
    return nullptr;

}

/**
 * Scans the cache to find the block, if the block is in the cache read from it
 * otherwise, remove a block from the cache, read the block from the disk and insert it to the
 * cache.
 * @param fd the file descriptor
 * @param currentBlockNumber the current block to be read
 * @param absPath the absolute path of the file
 * @param currentBlockBuffer the current buffer
 * @param offset the offset to begin reading
 * @param count how many bytes to be read
 * @return the number of bytes read
 */
int CacheAlgorithm::read(int fd,int currentBlockNumber, char* absPath ,void* currentBlockBuffer,
                         size_t count, off_t offset){
    BLOCK_ID currentBlockId =std::make_pair(fd,currentBlockNumber);
    Block * block = getBlockFromCache(fd,currentBlockNumber);
    if(block!= nullptr){
        return hitCache(currentBlockId, count,currentBlockBuffer , block, offset);
    }
    return missCache(currentBlockId ,count ,absPath,
                     offset,currentBlockBuffer );

}