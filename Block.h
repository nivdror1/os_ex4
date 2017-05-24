//
// Created by nivdror1 on 5/24/17.
//

#ifndef OS_EX4_BLOCK_H
#define OS_EX4_BLOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctime>

class Block {

private:

    /** The content of the block. */
    char* _blockInfo;

    /** The relative starting offset of the block. */
    off_t _blockOffset;

    /** Number of references for this block. */
    unsigned int _count;

    /** The last time that the block was used. */
    time_t _lastReadTime;

public:

    /**
     * Constructor
     * @param blockInfo the content of this block
     * @param blockOffset The relative starting offset of the block.
     * @param lastReadTime The last time that the block was used.
     */
    Block(char* blockInfo, off_t blockOffset, time_t lastReadTime);

    /**
     * Returns the offset of this block.
     * @return the offset of this block.
     */
    off_t getBlockOffset() const;

    /**
     * Returns the offset of this block.
     * @return the offset of this block.
     */
    unsigned int getCount() const;

    /**
     * Increment the number of times that this block was used by 1.
     */
    void incrementCount();

    /**
    * Returns the last time when this block was used.
    * @return the last time when this block was used.
    */
    time_t getLastReadTime() const;

    /**
     * Update the last time when this block was used.
     * @param _lastReadTime the last time when this block was used.
     */
    void setLastReadTime(time_t _lastReadTime);

};


#endif //OS_EX4_BLOCK_H
