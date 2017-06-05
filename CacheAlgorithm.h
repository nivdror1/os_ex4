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
	int numberOfBlocks;

	/**
	 * the block size
	 */
	size_t blockSize;

	/**
	* find the minimum block that is saved in the cache in order to remove it
	* @return a pair that consist of the fd and the block number
	*/
	virtual void eraseMinimum()=0;

	/**
	 * search the cache for the block, if found return the block
	 * @param fd the file descriptor
	 * @param currentBlockNumber the current block to be read
	 * @return upon success return the block , else return nullptr
	 */
	virtual Block* getBlockFromCache(int fd, int currentBlockNumber)const =0;

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
    int getNumberOfBlocks();

	/**
	 * increment the number of hits
	 */
	void incrementNumberOfHits();

	/**
	 * increment the number of misses
	 */
	 void incrementNumberOfMisses();

	/**
	 * search for the block in the cache, if the block is in the cache read from it
	 * else, remove a block from the cache, and read the block from the disk
	 * @param fd the file descriptorgetCacheBuffer()
	 * @param currentBlockNumber the current block to be read
	 * @param currentBlockBuffer the current buffer
	 * @param count the number of bytes to be read
	 * @param offset the offset to begin reading
	 * * @return the number of bytes read
	 */
	virtual int read(int fd,int currentBlockNumber, void* currentBlockBuffer, size_t count, off_t offset)=0;

	/**
	 * sort the cache into a vector of block
	 * @return a vector of all the blocks in the cache
	 */
	virtual std::list<BLOCK_ID> getOrderedCache()=0;

};


#endif //OS_EX4_CACHEALGORITHM_H
