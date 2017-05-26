//
// Created by nivdror1 on 5/24/17.
//

#ifndef OS_EX4_CACHEFILE_H
#define OS_EX4_CACHEFILE_H


#include "Block.h"
#include <vector>
#include <map>
#include <sys/types.h>

class CacheFile {
private:
	/** a container of the of blocks currently in the cache*/
	std::map<int,Block> fileBlocks;

	/** the file descriptor*/
	int fd;

    /** the absolute path of the file*/
	std::string absPath;

public:

	/**
	 * a c-tor
	 * @param fd the file descriptor
	 * @param absolutePath the absolute path of the file
	 * @return an object of CacheFile
	 */
	CacheFile(int fd,char *absolutePath);

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
	 * @param blockNumber the number of the block
	 * @return return true if such block exists else return false
	 */
	bool isBlockExists(int blockNumber);

	/**
	 * remove block from the vector fileBlocks
	 * @param blockNumber the number of the block
	 */
	bool removeBlock(int blockNumber);

	/**
	 * append a block to the vector fileBlocks
	 * @param block a new block to be appended to the vector
	 * @param blockNumber the number of the block
	 */
	void appendBlock(Block block, int blockNumber);

};


#endif //OS_EX4_CACHEFILE_H
