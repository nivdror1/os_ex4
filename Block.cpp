//
// Created by nivdror1 on 5/24/17.
//

#include "Block.h"
#include <cstring>
#include <stdlib.h>

/**
 * Constructor
 * @param blockInfo the content of this block
 * @param blockOffset The relative starting offset of the block.
 * @param lastReadTime The last time that the block was used.
 */
Block::Block(char* blockInfo, size_t blockSize, unsigned int blockNumber) :
        _blockNumber(blockNumber), _count(1)
{
    _blockInfo =(char*) aligned_alloc(blockSize, blockSize);
    memcpy(_blockInfo, blockInfo, blockSize);
}

/**
 * Returns the relative number of this block.
 * @return the relative number of this block.
 */
unsigned int Block::getBlockNumber() const
{
    return _blockNumber;
}

/**
 * Returns the offset of this block.
 * @return the offset of this block.
 */
unsigned int Block::getCount() const
{
    return _count;
}

/**
 * Increment the number of times that this block was used by 1.
 */
void Block::incrementCount()
{
    Block::_count += 1;
}

/**
* Returns the last time when this block was used.
* @return the last time when this block was used.
*/
time_t Block::getLastReadTime() const
{
    return _lastReadTime;
}

/**
 * Update the last time when this block was used.
 * @param _lastReadTime the last time when this block was used.
 */
void Block::setLastReadTime(time_t _lastReadTime)
{
    Block::_lastReadTime = _lastReadTime;
}
