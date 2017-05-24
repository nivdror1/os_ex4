//
// Created by nivdror1 on 5/24/17.
//

#include "CacheFile.h"


/**
 * a c-tor
 * @param fd the file descriptor
 * @return an object of CacheFile
 */
CacheFile::CacheFile(int fd):fd(fd){}

/**
 * a d-tor
 */
CacheFile::~CacheFile(){
	for(unsigned int i = 0; i< this->fileBlocks.size();i++){
		delete(fileBlocks.at(i));
	}
}

/**
 * get the the file descriptor
 * @return return the file descriptor
 */
int CacheFile::getFd(){
	return fd;
}

/**
 * check if a block exists on the cache
 * @param blockNumber the number of the block
 * @return return true if such block exists else return false
 */
bool CacheFile::isBlockExists(int blockNumber){
	auto it=fileBlocks.find(blockNumber);
	return it != fileBlocks.end();
}

/**
 * remove block from the vector fileBlocks
 * @param blockNumber the number of the block
 */
bool CacheFile::removeBlock(int blockNumber){
	auto it=fileBlocks.find(blockNumber);
	if(it != fileBlocks.end()){
		fileBlocks.erase(blockNumber);
		return true;
	}
	return false;
}

/**
 * append a block to the vector fileBlocks
 * @param blockNumber the number of the block
 */
void CacheFile::appendBlock(Block block,int blockNumber){
	fileBlocks.insert(std::make_pair(blockNumber,block));
}