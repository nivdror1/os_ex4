//
// Created by ido.shachar on 5/29/17.
//

#ifndef OS_EX4_CACHEALGORITHM_H
#define OS_EX4_CACHEALGORITHM_H

#include "Block.h"

class CacheAlgorithm
{
private:
	/** the cache*/
	std::map<int, std::vector<Block>> cacheBuffer ;

	/** number of hits*/
	int numberOfHits;

	/** number of misses*/
	int numberOfMisses;

public:

	/**
	 * d-tor
	 */
	virtual ~CacheAlgorithm();

	/**
	 * search for the block in the cache, if the block is in the cache read from it
	 * else, remove a block from the cache, and read the block from the disk
	 * @param fd the file descriptor
	 * @param currentBlockNumber the current block to be read
	 * @param currentBlockBuffer the current buffer
	 * @return the number of bytes read
	 */
	virtual int read(int fd,int currentBlockNumber, void* currentBlockBuffer);


};


#endif //OS_EX4_CACHEALGORITHM_H
