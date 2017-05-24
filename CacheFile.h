//
// Created by nivdror1 on 5/24/17.
//

#ifndef OS_EX4_CACHEFILE_H
#define OS_EX4_CACHEFILE_H


#include "Block.h"
#include <vector>
#include <sys/types.h>

class CacheFile {
private:
	/** a container of the of blocks currently in the cache*/
	std::vector<Block> fileBlocks;

	/** the file descriptor*/
	int fd;


public:

	/**
	 * a c-tor
	 * @param fd the file descriptor
	 * @param  firstBlock a block
	 * @return an object of CacheFile
	 */
	CacheFile(int fd,Block firstBlock);

	/**
	 * a d-tor
	 */
	~CacheFile();

	/**
	 * get the the file descriptor
	 * @return return the file descriptor
	 */
	int getFd();

	/**
	 * check if a block exists on the cache
	 * @param offset the offset from the initial bit of the file
	 * @return return true if such block exists else return false
	 */
	bool isBlockExists(off_t blockOffset);

	/**
	 * remove block from the vector fileBlocks
	 * @param blockOffset the offset from the initial bit of the file
	 */
	void removeBlock(off_t blockOffset);

	/**
	 * append a block to the vector fileBlocks
	 * @param block a new block to be appended to the vector
	 */
	void appendBlock(Block block);

};


#endif //OS_EX4_CACHEFILE_H
