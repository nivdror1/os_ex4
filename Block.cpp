//
// Created by nivdror1 on 5/24/17.
//

#include "Block.h"

/**
 * Constructor
 * @param blockInfo the content of this block
 * @param blockOffset The relative starting offset of the block.
 * @param lastReadTime The last time that the block was used.
 */
Block::Block(char* blockInfo,off_t blockOffset, time_t lastReadTime) : _blockOffset(
        blockOffset), _count(1), _lastReadTime(lastReadTime)
{
    _blockInfo = aligned_alloc(blockSize, blockSize);
    memcpy(_blockInfo, blockInfo, blockSize);
}

/**
 * Returns the offset of this block.
 * @return the offset of this block.
 */
off_t Block::getBlockOffset() const
{
    return _blockOffset;
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
