#include "LRUAlgo.h"
#include <algorithm>
#include <cstring>



/**
 * c-tor
 * @param blocks_num the number of blocks in the cache* get the map of the cache buffer
 * @param size the block size
 **/
LRUAlgo::LRUAlgo(int blocks_num, size_t size): CacheAlgorithm(blocks_num,size){}

/**
 * d-tor
 * delete all of the remaining block in the cache
 */
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

/**
 * after a hit has occured , update the orderedCache list hence update the cache
 * so it will remain sorted. in order to do that we search for the currentBlockId
 * and then push it at the back of the list
 * @param currentBlockId a file descriptor and the number of block in that file
 */
void LRUAlgo::updateCacheAfterHit(BLOCK_ID currentBlockId){
	//search and move the block id to end of the list
	auto searchedBlockId = std::find(orderedCache.begin(),
									 orderedCache.end(), currentBlockId);
	orderedCache.erase(searchedBlockId);
	orderedCache.push_back(currentBlockId);
}

/**
 * insert a block into the cache
 * @param currentBlockId a file descriptor and the number of block in that file
 * @param block the new inserted block into the cache
 */
void LRUAlgo::insertNewBlockToCache(BLOCK_ID currentBlockId,Block* block){
	cacheBuffer.insert(std::pair<BLOCK_ID,Block*> (currentBlockId,block));
	orderedCache.push_back(currentBlockId);
}

/**
 * get the sortedCache
 * @return a list of all the blocks in the cache
 */
std::list<BLOCK_ID> LRUAlgo::getOrderedCache(){
	return orderedCache;
}

