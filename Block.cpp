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
 */
Block::Block(void* blockInfo, size_t blockSize) : _count(1)
{
    _blockInfo = aligned_alloc(blockSize, blockSize);
    memcpy(_blockInfo, blockInfo, blockSize);
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
