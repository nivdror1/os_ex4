#include "LRU.h"


/**
 * c-tor
 * @param blocks_num the number of blocks in the cache* get the map of the cache buffer
 **/
LRU::LRU(int blocks_num): numberOfBlocks(blocks_num){}

/**
 * get the map of the cache buffer
 * @return the cache buffer
 */
std::map<std::pair<int,int> ,Block* > LRU::getCacheBuffer(){
    return this->cacheBuffer;
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
 * @param time the time of which the first block was last read
 * @param otherTime the time of which the second block was last read
 * @return true if time> otherTime else return false
 */
bool LRU::compare(const time_t time, const  time_t otherTime) const {
    if (difftime(time,otherTime)> 0){
        return true;
    }
    return false;
}
/**
 * find the minimum block that is saved in the cache in order to remove it
 * @return a pair that consist of the fd and the block number
 */
 std::pair<int, int> LRU::findMinimum(){
    auto min = std::min_element(getCacheBuffer().begin(),getCacheBuffer().end(),&compare);
    //todo check whether min_element function has failed
    getCacheBuffer().erase(min);

};


/**
 * search the cache for the block, if found return the block
 * @param fd the file descriptor
 * @param currentBlockNumber the current block to be read
 * @return upon success return the block , else return nullptr
 */
Block* CacheAlgorithm::getBlockFromCache(int fd, int currentBlockNumber){
    auto searchedBlock = getCacheBuffer().find(std::make_pair(fd,currentBlockNumber));
    if(searchedBlock!=getCacheBuffer().end()){
        incrementNumberOfHits();
        return searchedBlock->second;
    }else{
        if(getCacheBuffer().size()==getNumberOfBlocks()){
            findMinimum();
        }
        incrementNumberOfMisses();
        //todo read the block
    }

}


