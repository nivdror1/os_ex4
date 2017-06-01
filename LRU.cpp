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
 * when there is hit, read a block or a part of it from the cache
 * and relocated the block id to the end of list
 * @param currentBlockId the file descrioptor and the block number
 * @param count count how many bytes to be read
 * @param currentBlockBuffer the current buffer
 * @param block a block object
 * @param offset offset the offset to begin reading
 * @return the number of bytes read
 */
int LRU::hitCache(BLOCK_ID currentBlockId, size_t count,void* currentBlockBuffer ,Block * block, off_t offset) {
	//move the block id to end of the list

	auto searchedBlockId = std::find(orderedCache.begin(),
	                                 orderedCache.end(), currentBlockId);
	orderedCache.erase(searchedBlockId);
	orderedCache.push_back(currentBlockId);

	incrementNumberOfHits(); //increment the hit number
	//read from the cache
	return block->getPartOfBlockContent(currentBlockBuffer, offset, count);
}

/**
 * when there is a miss, if the cache is full erase the minimum.
 * read the block from the disk into the cache, and fill the current
 * @param currentBlockId the file descrioptor and the block number
 * @param count count how many bytes to be read
 * @param block block a block object
 * @param offset offset the offset to begin reading
 * @param fileInfo a stat object reperesented the file info
 * @return the number of bytes read
 */
int LRU::missCache(BLOCK_ID currentBlockId ,size_t count ,Block* block,
                   off_t offset,struct stat *fileInfo,void *currentBlockBuffer ){
	if(cacheBuffer.size()==getNumberOfBlocks()){
		eraseMinimum();
	}
	incrementNumberOfMisses();

	if(pread(currentBlockId.first,currentBlockBuffer,this->blockSize,offset-(offset% blockSize))==-1){
		return -1;
	}

	try {
		block = new Block(currentBlockBuffer,blockSize, currentBlockId.second, fileInfo);
	}catch (std::bad_alloc e){
		//todo error maybe throw exception
	}
	cacheBuffer.insert(std::pair<BLOCK_ID,Block*> (currentBlockId,block));

	return block->getPartOfBlockContent(currentBlockBuffer,offset,count);
}
/**
 * search for the block in the cache, if the block is in the cache read from it
 * else, remove a block from the cache, and read the block from the disk
 * @param fd the file descriptorgetCacheBuffer()
 * @param currentBlockNumber the current block to be read
 * @param currentBlockBuffer the current buffer
 * @param offset the offset to begin reading
 * @param count how many bytes to be read
 * @param fileInfo a stat object reperesented the file info
 * @return the number of bytes read
 */
int LRU::read(int fd,int currentBlockNumber, void* currentBlockBuffer,size_t count, off_t offset, struct stat *fileInfo){

	BLOCK_ID currentBlockId =std::make_pair(fd,currentBlockNumber);
	Block * block= getBlockFromCache(fd,currentBlockNumber);
	if(block!= nullptr){
		return hitCache(currentBlockId, count,currentBlockBuffer , block, offset);
	}
	return missCache(currentBlockId ,count ,block,
			offset,fileInfo,currentBlockBuffer );

}

/**
 * sort the cache into a vector of block
 * @return a vector of all the blocks in the cache
 */
std::list<BLOCK_ID> LRU::getOrderedCache(){
	return orderedCache;
}

