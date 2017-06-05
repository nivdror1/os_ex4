//
// Created by nivdror1 on 5/24/17.
//

#include "Block.h"
#include <cstring>
#include <stdlib.h>
#include <sys/stat.h>
#include <algorithm>
#include <zconf.h>

/**
 * Constructor
 * @param blockInfo the content of this block
 * @param blockOffset The relative starting offset of the block.
 */
Block::Block(void* blockInfo, size_t blockSize, int currentBlockNumber ,int fd) :
        _count(1), _currentBlockNumber(currentBlockNumber), _fd(fd), _state(New)
{ //todo i had passec the fd for lseek (to get the size of file) and for fstat
    _blockInfo = (char*)aligned_alloc(blockSize, blockSize); //todo do we really need to use aligned alloc
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
    struct stat st;
    fstat(this->_fd,&st); //todo error
    off_t fileSize= lseek(_fd,0,SEEK_END); //todo error

    off_t numberOfReadBytes;
    if ((fileSize/st.st_blksize) == _currentBlockNumber){
        numberOfReadBytes = std::min(fileSize-offset, (off_t)count);
    }
    else {
        numberOfReadBytes = std::min(((_currentBlockNumber+1)*st.st_blksize)-offset, (off_t)count);
    }
    memcpy(buffer, _blockInfo + (offset%st.st_blksize), (size_t)numberOfReadBytes);
    return (int)numberOfReadBytes;
}

/**
 * Returns the current section that this block belongs to.
 * @return the current section that this block belongs to.
 */
State Block::getState() const
{
    return _state;
}

/**
 * Sets the current section that this block belongs to the given state.
 * @param state the new section that this block belongs to.
 */
void Block::setState(State state)
{
    Block::_state = state;
}
