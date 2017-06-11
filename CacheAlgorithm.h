//
// Created by ido.shachar on 5/29/17.
//

#ifndef OS_EX4_CACHEALGORITHM_H
#define OS_EX4_CACHEALGORITHM_H

#include <utility>
#include "Block.h"
#include <vector>
#include <sys/stat.h>
#include <list>
#include <map>

typedef std::pair<int,int> BLOCK_ID;

struct CompareItems{
    /**
    * a functor whom compares the cache map by first comparing the fd
    * and comparing the block number
    */
    bool operator()(const std::pair<int,int> key , const std::pair<int,int>  otherKey) const{
        if (key.first < otherKey.first){
            return true;
        }else if (key.first == otherKey.first) {
            return key.second < otherKey.second;
        }
        return false;
    }
};

class CacheAlgorithm
{
private:


	/** number of hits*/
	int numberOfHits;

	/** number of misses*/
	int numberOfMisses;

protected:

	/** number of blocks in the cache*/
	unsigned int numberOfBlocks;

	/**
	 * the block size
	 */
	size_t blockSize;

    /** the cache */
    std::map<BLOCK_ID,Block*, CompareItems> cacheBuffer ;

    /**
	* find the minimum block that is saved in the cache in order to remove it
	* @return a pair that consist of the fd and the block number
	*/
    virtual void eraseMinimum()=0;

	/**
     * Updates the relevant elements in cache after an hit occurred.
    */
    virtual void updateCacheAfterHit(BLOCK_ID currentBlockId)=0;

	/**
	 * Checks if the cache if full, if so remove the minimum element from it.
	 */
	virtual void updateCacheAfterMiss();

	/**
	 * Insert the given block with given details to the cache.
	 * @param currentBlockId  pair of file descriptor and block number
	 * @param block the block to insert to the cache
	 */
	virtual void insertNewBlockToCache(BLOCK_ID currentBlockId,Block* block)=0;

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
    int hitCache(BLOCK_ID currentBlockId, size_t count,void* currentBlockBuffer ,Block * block,off_t offset);

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
    int missCache(BLOCK_ID currentBlockId ,size_t count ,char* absPath,
                  off_t offset,void *currentBlockBuffer);

public:
    /**
     * c-tor
     * @param blocks_num the number of blocks in the cache
     * @param size the block size
     */
    CacheAlgorithm(int blocks_num,size_t size);

	/**
	 * d-tor
	 */
	virtual ~CacheAlgorithm() {}

	/**
	 * get the number of hits
	 * @return return the number of hits
	 */
	int getNumberOfHits();

	/**
	 * get the number of misses
	 * @return return the number of misses
	 */
	int getNumberOfMisses();

    /**
	 * get the number of blocks in the cache
	 * @return return the number of blocks in the cache
	 */
    unsigned int getNumberOfBlocks();

	/**
	 * increment the number of hits
	 */
	void incrementNumberOfHits();

	/**
	 * increment the number of misses
	 */
	 void incrementNumberOfMisses();

	/**
	 * Scans the cache to find the block that match to the given file descriptor and block number.
	 * @param fd the file descriptor
	 * @param currentBlockNumber the current block to be read
	 * @return upon success return the block , else return nullptr
	 */
	virtual Block* getBlockFromCache(int fd, int currentBlockNumber)const;

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
	int read(int fd,int currentBlockNumber, char* absPath,void* currentBlockBuffer, size_t count, off_t offset);

	/**
	 * sort the cache into a vector of block
	 * @return a vector of all the blocks in the cache
	 */
	virtual std::list<BLOCK_ID> getOrderedCache()=0;

};


#endif //OS_EX4_CACHEALGORITHM_H
