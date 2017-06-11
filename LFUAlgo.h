//
// Created by nivdror1 on 5/24/17.
//

#ifndef OS_EX4_LFU_H
#define OS_EX4_LFU_H

#include "CacheAlgorithm.h"
#include <map>


struct CompareItemsByFrequentlyUsed{
    /**
    * a functor whom compares the cache map by the frequently of block usage
    */
    bool operator()(const std::pair<BLOCK_ID, Block*>& left , const std::pair<BLOCK_ID, Block*>&right) const{
        return left.second->getCount() < right.second->getCount();
    }
};

class LFUAlgo: public CacheAlgorithm {

public:
    /**
     * c-tor
     * @param blocks_num the number of blocks in the cache
     * @param size the block size
     */
    LFUAlgo(int blocks_num,size_t size);

    /**
     * d-tor
     */
    ~LFUAlgo();


    /**
	* find the minimum block that is saved in the cache in order to remove it
	* @return a pair that consist of the fd and the block number
	*/
    void eraseMinimum();

    /**
     * Updates the relevant elements in cache after an hit occurred.
	*/
    void updateCacheAfterHit(BLOCK_ID currentBlockId);

    /**
     * Inserts the given block with given details to the cache.
     * @param currentBlockId  pair of file descriptor and block number
     * @param block the block to insert to the cache
     */
    void insertNewBlockToCache(BLOCK_ID currentBlockId,Block* block);

    /**
     * sort the cache into a vector of block
     * @return a vector of all the blocks in the cache
     */
    std::list<BLOCK_ID> getOrderedCache();
};


#endif //OS_EX4_LFU_H
