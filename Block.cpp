

#include "Block.h"
#include <cstring>
#include <sys/stat.h>
#include <algorithm>
#include <zconf.h>
#include <malloc.h>
#include <stdlib.h>

/**
 * Constructor
 * @param blockInfo the content of this block
 * @param blockOffset The relative starting offset of the block.
 */
Block::Block(void* blockInfo, size_t blockSize, int currentBlockNumber ,int fd, char* absPath) :
        _count(1), _currentBlockNumber(currentBlockNumber), _fd(fd),_state(New)
{
    _blockInfo = (char*)aligned_alloc(blockSize, blockSize);
    memcpy(_blockInfo, blockInfo, blockSize);

    this->_absPath = (char*)malloc(strlen(absPath)+1);
    memcpy(_absPath,absPath, strlen(absPath)+1);
}

/**
 * d-tor
 */
Block::~Block() {
    free(_blockInfo);
    free(_absPath);
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
 * decide what the size of the current block is need to be read,
 * and at the end write the specified amount into the buffer
 * @param buffer the buffer to be written into
 * @param offset an offset from the beginning of the block in order to begin reading
 * @param count  the number of bytes to read
 * @return
 */
int Block::getPartOfBlockContent(void *buffer, off_t offset, size_t count)
{
    struct stat st;
    if(fstat(this->_fd,&st) != -1){
	    // get the size of the file
	    if(off_t fileSize= lseek(_fd,0,SEEK_END)!= -1){
		    off_t numberOfReadBytes;
		    //handle the case of the current block is the last block
		    if ((fileSize/st.st_blksize) == _currentBlockNumber){
			    numberOfReadBytes = std::min(fileSize-offset, (off_t)count);
		    }
		    else {
			    numberOfReadBytes = std::min(((_currentBlockNumber+1)*st.st_blksize)-offset, (off_t)count);
		    }
		    //write the desired content of the block to the buffer
		    memcpy(buffer, _blockInfo + (offset%st.st_blksize), (size_t)numberOfReadBytes);
		    return (int)numberOfReadBytes;
	    }
    }
	return -1;

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

/**
 * get the absolute path
 * @return return the absolute path
 */
char *Block::get_absPath() const
{
    return _absPath;
}
