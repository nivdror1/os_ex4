//
// Created by nivdror1 on 5/24/17.
//

#include "CacheFile.h"


/**
 * a c-tor
 * @param fd the file descriptor
 * @param firstBlock a block
 * @return an object of CacheFile
 */
CacheFile::CacheFile(int fd,Block firstBlock):fd(fd){
	appendBlock(firstBlock);

}

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
 * @param offset the offset from the initial bit of the file
 * @return return true if such block exists else return false
 */
bool CacheFile::isBlockExists(off_t blockOffset){

}

/**
 * remove block from the vector fileBlocks
 * @param blockOffset the offset from the initial bit of the file
 */
void removeBlock(off_t blockOffset);

/**
 * append a block to the vector fileBlocks
 */
void appendBlock(off_t blockOffset);