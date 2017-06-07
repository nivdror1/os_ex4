#include "LRUAlgo.h"
#include <algorithm>
#include <zconf.h>
#include <cstring>
#include <sys/time.h>


/**
 * c-tor
 * @param blocks_num the number of blocks in the cache* get the map of the cache buffer
 * @param size the block size
 **/
LRUAlgo::LRUAlgo(int blocks_num, size_t size): CacheAlgorithm(blocks_num,size){}

LRUAlgo::~LRUAlgo(){
	for(auto iter= cacheBuffer.begin();iter!= cacheBuffer.end();iter++){
		delete (*iter).second;
	}
}

/**
 * find the minimum block that is saved in the cache in order to remove it
 * @return a pair that consist of the fd and the block number
 */
 void LRUAlgo::eraseMinimum(){
	BLOCK_ID curBlock =orderedCache.front();
	//search and delete it from the cache map
	auto searchedBlock = cacheBuffer.find(std::make_pair(curBlock.first,curBlock.second));
	if(searchedBlock!=cacheBuffer.end()){
		 delete searchedBlock->second;
		cacheBuffer.erase(searchedBlock);
	}
	//remove from the ordered cache list
	orderedCache.pop_front();

}

void LRUAlgo::updateCacheAfterHit(BLOCK_ID currentBlockId){
	//move the block id to end of the list

	auto searchedBlockId = std::find(orderedCache.begin(),
									 orderedCache.end(), currentBlockId);
	orderedCache.erase(searchedBlockId);
	orderedCache.push_back(currentBlockId);
}

void LRUAlgo::insertNewBlockToCache(BLOCK_ID currentBlockId,Block* block){
	cacheBuffer.insert(std::pair<BLOCK_ID,Block*> (currentBlockId,block));
	orderedCache.push_back(currentBlockId);
}

/**
 * sort the cache into a vector of block
 * @return a vector of all the blocks in the cache
 */
std::list<BLOCK_ID> LRUAlgo::getOrderedCache(){
	return orderedCache;
}

