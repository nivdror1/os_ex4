//
// Created by nivdror1 on 5/24/17.
//

#ifndef OS_EX4_LRU_H
#define OS_EX4_LRU_H

#include "Block.h"
#include "CacheAlgorithm.h"
#include <map>


class LRU: public CacheAlgorithm {

private:
    /** the cache */
    std::map<BLOCK_ID,Block*,LRU > cacheBuffer ; //todo not sure about the map definition
	/**
	 * a vector of BLOCK_ID that sorted
	 */
	std::list<BLOCK_ID> orderedCache;

public:
    /**
     * c-tor
     * @param blocks_num the number of blocks in the cache
     * @param blockSize the block size
     */
    LRU(int blocks_num,size_t blockSize);

	/**
	 * d-tor
	 */
	 ~LRU();


    /**
    * a functor whom compares the cache map by the operator <
    */
    bool operator()(const BLOCK_ID key , const BLOCK_ID otherKey) const;

    /**
     * compare the time of two blocks in the cache,
     * this comparison simulates the process of the LRU
     * @param firstBlock a block id
     * @param secondBlock a block id
     * @return true if time> otherTime else return false
     */
    bool compare(const BLOCK_ID firstBlock, const BLOCK_ID secondBlock) const ;

    /**
	* find the minimum block that is saved in the cache in order to remove it
	* @return a pair that consist of the fd and the block number
	*/
    void eraseMinimum();

    /**
	 * search the cache for the block, if found return the block
	 * @param fd the file descriptor
	 * @param currentBlockNumber the current block to be read
	 * @return upon success return the block , else return nullptr
	 */
     Block* getBlockFromCache(int fd, int currentBlockNumber) const;

	/**
	 * search for the block in the cache, if the block is in the cache read from it
	 * else, remove a block from the cache, and read the block from the disk
	 * @param fd the file descriptorgetCacheBuffer()
	 * @param currentBlockNumber the current block to be read
	 * @param currentBlockBuffer the current buffer
     * @param count the number of bytes to be read
     * @param offset the offset to begin reading
     * @param fileInfo a stat object reperesented the file info
	 * @return the number of bytes read
	 */
	int read(int fd,int currentBlockNumber, void* currentBlockBuffer,size_t count,off_t offset, stat *fileInfo);

	/**
	 * sort the cache into a vector of block
	 * @return a vector of all the blocks in the cache
	 */
	std::list<BLOCK_ID> getOrderedCache();
};

#endif //OS_EX4_LRU_H
