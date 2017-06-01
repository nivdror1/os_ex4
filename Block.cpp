//
// Created by nivdror1 on 5/24/17.
//

#include "Block.h"
#include <cstring>
#include <stdlib.h>
#include <sys/stat.h>
#include <algorithm>

/**
 * Constructor
 * @param blockInfo the content of this block
 * @param blockOffset The relative starting offset of the block.
 */
Block::Block(void* blockInfo, size_t blockSize, int currentBlockNumber ,struct stat *fileInfo) :
        _count(1), _currentBlockNumber(currentBlockNumber), _fileInfo(fileInfo)
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

/**
 *
 * @param buffer
 * @param offset
 * @param count
 * @return
 */
int Block::getPartOfBlockContent(void *buffer, off_t offset, size_t count)
{
    off_t numberOfReadBytes;
    if ((_fileInfo->st_size/_fileInfo->st_blksize) == _currentBlockNumber){
        numberOfReadBytes = std::min(_fileInfo->st_size-offset, (off_t)count);
    }
    else {
        numberOfReadBytes = std::min(((_currentBlockNumber+1)*_fileInfo->st_blksize)-offset, (off_t)count);
    }
    memcpy(buffer, _blockInfo, (size_t)numberOfReadBytes);
    return (int)numberOfReadBytes;
}
