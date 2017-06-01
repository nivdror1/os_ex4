#include "LRU.h"
#include <algorithm>
#include <zconf.h>
#include <cstring>
#include <sys/time.h>


/**
 * c-tor
 * @param blocks_num the number of blocks in the cache* get the map of the cache buffer
 * @param blockSize the block size
 **/
LRU::LRU(int blocks_num, size_t blockSize): numberOfBlocks(blocks_num),blockSize(blockSize){}

LRU::~LRU(){
	for(auto iter= cacheBuffer.begin();iter!= cacheBuffer.end();iter++){
		delete (*iter).second;
	}
}


/**
* a functor whom compares the cache map by first comparing the fd
* and comparing the block number
*/
bool LRU::operator()(const std::pair<int,int> key , const std::pair<int,int>  otherKey) const{
    if (key.first < otherKey.first){
        return true;
    }else{
        return key.second < otherKey.second;
    }
}

/**
 * compare the time of two blocks in the cache,
 * this comparison simulates the process of the LRU
 * @param firstBlock a block id
 * @param secondBlock a block id
 * @return true if time> otherTime else return false
 */
bool LRU::compare(const std::pair<int,int> firstBlock, const std::pair<int,int> secondBlock ) const {
	time_t firstTime = getBlockFromCache(firstBlock.first,firstBlock.second)->getLastReadTime();
	time_t secondTime = getBlockFromCache(secondBlock.first,secondBlock.second)->getLastReadTime();
	return difftime(firstTime, secondTime) > 0;
}
/**
 * find the minimum block that is saved in the cache in order to remove it
 * @return a pair that consist of the fd and the block number
 */
 void LRU::eraseMinimum(){
	BLOCK_ID curBlock =orderedCache.front();
	//search and delete it from the cache map
	auto searchedBlock = cacheBuffer.find(std::make_pair(curBlock.first,curBlock.second));
	if(searchedBlock!=cacheBuffer.end()){
		 delete searchedBlock->second;
	}
	//remove from the ordered cache list
	orderedCache.pop_front();

};


/**
 * search the cache for the block, if found return the block
 * @param fd the file descriptor
 * @param currentBlockNumber the current block to be read
 * @return upon success return the block , else return nullptr
 */
Block* LRU::getBlockFromCache(int fd, int currentBlockNumber) const{
    auto searchedBlock = cacheBuffer.find(std::make_pair(fd,currentBlockNumber));
    if(searchedBlock!=cacheBuffer.end()){
        return searchedBlock->second;
    }
	return nullptr;

}

/**
 * search for the block in the cache, if the block is in the cache read from it
 * else, remove a block from the cache, and read the block from the disk
 * @param fd the file descriptorgetCacheBuffer()
 * @param currentBlockNumber the current block to be read
 * @param currentBlockBuffer the current buffer
 * @param offset the offset to begin reading
 * @param fileInfo a stat object reperesented the file info
 * @return the number of bytes read
 */
int LRU::read(int fd,int currentBlockNumber, void* currentBlockBuffer,size_t count, off_t offset, stat *fileInfo){

	BLOCK_ID currentBlockId =std::make_pair(fd,currentBlockNumber);
	Block * block= getBlockFromCache(fd,currentBlockNumber);
	if(block!= nullptr){
		//move the block id to end of the list

		auto searchedBlockId = std::find(orderedCache.begin(),
		                                 orderedCache.end(),currentBlockId);
		orderedCache.erase(searchedBlockId);
		orderedCache.push_back(currentBlockId);

		incrementNumberOfHits(); //increment the hit number
		//read from the cache
		if(count> blockSize){
			memcpy(currentBlockBuffer, block, blockSize);
			return (int)blockSize;
		}
		memcpy(currentBlockBuffer, block, count);
		return (int)count;
	}
	if(cacheBuffer.size()==getNumberOfBlocks()){
		eraseMinimum();
	}
	incrementNumberOfMisses();

	void* tempBuffer = nullptr;

	pread(fd,tempBuffer,this->blockSize,offset-(offset% blockSize));

	try {
		block = new Block(tempBuffer,blockSize, currentBlockNumber, fileInfo);
	}catch (std::bad_alloc e){
		//todo error maybe throw exception
	}
	cacheBuffer.insert(std::pair<BLOCK_ID,Block*> (currentBlockId,block));

	return block->getPartOfBlockContent(tempBuffer,offset,count);
}

/**
 * sort the cache into a vector of block
 * @return a vector of all the blocks in the cache
 */
std::list<BLOCK_ID> LRU::getOrderedCache(){
	return orderedCache;
}

