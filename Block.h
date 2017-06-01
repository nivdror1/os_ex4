//
// Created by nivdror1 on 5/24/17.
//

#ifndef OS_EX4_BLOCK_H
#define OS_EX4_BLOCK_H


#include <stdlib.h>
#include <sys/types.h>
#include <ctime>

class Block {

private:

    /** The content of the block. */
    void* _blockInfo;

    /** Number of references for this block. */
    unsigned int _count;

    int _currentBlockNumber;

    /** Information struct of the file that this block is part of */
    struct stat *_fileInfo;

public:

    /**
     * Constructor
     * @param blockInfo the content of this block
     * @param blockOffset The relative starting offset of the block.
     * @param fileInfo Information struct of the file that this block is part of
     */
    Block(void* blockInfo, size_t blockSize, int currentBlockNumber, struct stat *fileInfo);

    /**
     * Returns the offset of this block.
     * @return the offset of this block.
     */
    unsigned int getCount() const;

    /**
     * Increment the number of times that this block was used by 1.
     */
    void incrementCount();

    int getPartOfBlockContent(void* buffer, off_t offset, int count);

};


#endif //OS_EX4_BLOCK_H
