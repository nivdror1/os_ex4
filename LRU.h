//
// Created by nivdror1 on 5/24/17.
//

#ifndef OS_EX4_LRU_H
#define OS_EX4_LRU_H

#include "Block.h"
#include "CacheAlgorithm.h"
#include <map>

class LRU: public CacheAlgorithm {

private:
    /** the cache */
    std::map<std::pair<int,int> ,Block*,LRU > cacheBuffer ; //todo not sure about the map definition


public:
    /**
     * c-tor
     * @param blocks_num the number of blocks in the cache
     */
    LRU(int blocks_num);

    /**
     * get the map of the cache buffer
     * @return the cache buffer
     */
    std::map<std::pair<int,int> ,Block* ,LRU> getCacheBuffer();

    /**
    * a functor whom compares the cache map by the operator <
    */
    bool operator()(const std::pair<int,int> key , const std::pair<int,int>  otherKey) const;

    /**
     * compare the time of two blocks in the cache,
     * this comparison simulates the process of the LRU
     * @param time the time of which the first block was last read
     * @param otherTime the time of which the second block was last read
     * @return true if time> otherTime else return false
     */
    bool compare(const time_t time, const  time_t otherTime) const ;

    /**
	* find the minimum block that is saved in the cache in order to remove it
	* @return a pair that consist of the fd and the block number
	*/
    virtual std::pair<int, int> findMinimum();

    /**
	 * search the cache for the block, if found return the block
	 * @param fd the file descriptor
	 * @param currentBlockNumber the current block to be read
	 * @return upon success return the block , else return nullptr
	 */
    virtual Block* getBlockFromCache(int fd, int currentBlockNumber);
};

#endif //OS_EX4_LRU_H
