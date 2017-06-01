//
// Created by ido.shachar on 5/29/17.
//

#include "CacheAlgorithm.h"


/**
	 * get the number of hits
	 * @return return the number of hits
	 */
int CacheAlgorithm::getNumberOfHits(){
    return this->numberOfHits;
}

/**
 * get the number of misses
 * @return return the number of misses
 */
int CacheAlgorithm::getNumberOfMisses() {
    return this->numberOfMisses;
}

/**
 * increment the number of hits
 */
void CacheAlgorithm::incrementNumberOfHits(){
    this->numberOfHits++;
}

/**
 * increment the number of misses
 */
void CacheAlgorithm::incrementNumberOfMisses(){
    this->numberOfMisses++;
}

/**
 * get the number of blocks in the cache
 * @return return the number of blocks in the cache
 */
int CacheAlgorithm::getNumberOfBlocks(){
    return this->numberOfBlocks;
}