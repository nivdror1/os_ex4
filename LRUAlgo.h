//
// Created by nivdror1 on 5/24/17.
//

#ifndef OS_EX4_LRU_H
#define OS_EX4_LRU_H

#include "Block.h"
#include "CacheAlgorithm.h"
#include <map>



class LRUAlgo: public CacheAlgorithm {

private:
	/**
	 * a vector of BLOCK_ID that sorted
	 */
	std::list<BLOCK_ID> orderedCache;

public:
    /**
     * c-tor
     * @param blocks_num the number of blocks in the cache
     * @param size the block size
     */
    LRUAlgo(int blocks_num,size_t size);

	/**
	 * d-tor
	 */
	 ~LRUAlgo();


    /**
	* find the minimum block that is saved in the cache in order to remove it
	* @return a pair that consist of the fd and the block number
	*/
    void eraseMinimum();

	void updateCacheAfterHit(BLOCK_ID currentBlockId);

	void insertNewBlockToCache(BLOCK_ID currentBlockId,Block* block);

	/**
	 * sort the cache into a vector of block
	 * @return a vector of all the blocks in the cache
	 */
	std::list<BLOCK_ID> getOrderedCache();
};

#endif //OS_EX4_LRU_H
