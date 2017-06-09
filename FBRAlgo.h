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

    /** maximum number of elements in old section. */
    unsigned int _sizeOfOldSection;

    /** maximum number of elements in new section. */
    unsigned int _sizeOfNewSection;

    /** iterator that points to the element next to the last element in old section. */
    std::list<BLOCK_ID>::iterator _endOfOldSection;

    /** iterator that points to the element next to the last element in middle section. */
    std::list<BLOCK_ID>::iterator _endOfMiddleSection;

    /**
     * Updates the state of the last element that enters to the old section.
     * @param remainingOldSectionSize number of elements in the old section
     */
    void updateLastOldSectionItem(int remainingOldSectionSize);

    /**
     * Updates the state of the last element that enters to the middle section.
     * @param remainingOldSectionSize number of elements in old section
     * @param middleSectionSize maximum number of elements in middle section
     * @param remainingMiddleSectionSize number of elements in middle section
     */
    void updateLastMiddleSectionItem(int remainingOldSectionSize, int middleSectionSize, int remainingMiddleSectionSize);

    /**
     * Updates the states of new elements in old and middle section
     */
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

    /**
     * Updates the relevant elements in cache after an hit occurred.
	*/
    void updateCacheAfterHit(BLOCK_ID currentBlockId);

    /**
     * Checks if the cache if full, if so remove the minimum element from it.
     */
    void updateCacheAfterMiss();

    /**
     * Inserts the given block with given details to the cache.
     * @param currentBlockId  pair of file descriptor and block number
     * @param block the block to insert to the cache
     */
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
