//
// Created by ido.shachar on 6/5/17.
//

#ifndef OS_EX4_FBRALGO_H
#define OS_EX4_FBRALGO_H


#include "Block.h"
#include "CacheAlgorithm.h"
#include <map>

class FBRAlgo: public CacheAlgorithm {

private:

    /**
     * a vector of BLOCK_ID that sorted
     */
    std::list<BLOCK_ID> orderedCache;

    unsigned int _sizeOfOldSection;

    unsigned int _sizeOfNewSection;

    std::list<BLOCK_ID>::iterator _endOfOldSection;

    std::list<BLOCK_ID>::iterator _endOfMiddleSection;

    void updateLastOldSectionItem(int remainingOldSectionSize);

    void updateLastMiddleSectionItem(int remainingOldSectionSize, int middleSectionSize, int remainingMiddleSectionSize);

    void changeState();

public:
    /**
     * c-tor
     * @param blocks_num the number of blocks in the cache
     * @param size the block size
     * @param f_old
     * @param f_new
     */
    FBRAlgo(int blocks_num,size_t size, double f_old , double f_new );

    /**
     * d-tor
     */
    ~FBRAlgo();


    /**
	* find the minimum block that is saved in the cache in order to remove it
	* @return a pair that consist of the fd and the block number
	*/
    void eraseMinimum();

    void updateCacheAfterHit(BLOCK_ID currentBlockId);

    void updateCacheAfterMiss();

    void insertNewBlockToCache(BLOCK_ID currentBlockId,Block* block);

    /**
	 * search the cache for the block, if found return the block
	 * @param fd the file descriptor
	 * @param currentBlockNumber the current block to be read
	 * @return upon success return the block , else return nullptr
	 */
    Block* getBlockFromCache(int fd, int currentBlockNumber) const;

    /**
     * sort the cache into a vector of block
     * @return a vector of all the blocks in the cache
     */
    std::list<BLOCK_ID> getOrderedCache();
};



#endif //OS_EX4_FBRALGO_H
