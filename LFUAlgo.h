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
    bool operator()(const std::pair<BLOCK_ID, Block*> left , const std::pair<BLOCK_ID, Block*> right) const{
        return left.second->getCount() < right.second->getCount();
    }
};

class LFUAlgo: public CacheAlgorithm {

private:
    /** the cache */
    std::map<BLOCK_ID,Block*, CompareItems> cacheBuffer ;


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
    int hitCache(BLOCK_ID currentBlockId, size_t count,void* currentBlockBuffer ,Block * block,off_t offset);

    /**
     * when there is a miss, if the cache is full erase the minimum.
     * read the block from the disk into the cache, and fill the current
     * @param currentBlockId the file descrioptor and the block number
     * @param count count how many bytes to be read
     * @param block block a block object
     * @param offset offset the offset to begin reading
     * @return the number of bytes read
     */
    int missCache(BLOCK_ID currentBlockId ,size_t count ,Block* block,
                  off_t offset,void *currentBlockBuffer);


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
     * @return the number of bytes read
     */
    int read(int fd,int currentBlockNumber, void* currentBlockBuffer,size_t count,off_t offset);

    /**
     * sort the cache into a vector of block
     * @return a vector of all the blocks in the cache
     */
    std::list<BLOCK_ID> getOrderedCache();
};


#endif //OS_EX4_LFU_H
