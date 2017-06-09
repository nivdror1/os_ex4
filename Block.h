
#ifndef OS_EX4_BLOCK_H
#define OS_EX4_BLOCK_H


#include <stdlib.h>
#include <sys/types.h>
#include <ctime>

enum State {Old,New,Middle};

class Block {

private:

    /** The content of the block. */
    char* _blockInfo;

    /** Number of references for this block. */
    unsigned int _count;

    /** the number of the current block */
    int _currentBlockNumber;

    /** the file descriptor*/
    int _fd;

	/** the absolute path of the file*/
    char* _absPath;

    /** Flag that shows in which section this block belongs  */
    State _state;

public:

    /**
     * Constructor
     * @param blockInfo the content of this block
     * @param blockOffset The relative starting offset of the block.
     * @param fd the file descriptor
     */
    Block(void* blockInfo, size_t blockSize, int currentBlockNumber, int fd, char* absPath);

    /**
     * d-tor
     */
    ~Block();

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
     * Insert to the given buffer the content in the block that starts with offset of offset and
     * copy count amount of bytes.
     * @param buffer the buffer to copy to.
     * @param offset the starting offset of the copy
     * @param count number of bytes to copy
     * @return number of bytes that actually copied
     */
    int getPartOfBlockContent(void* buffer, off_t offset, size_t count);

    /**
     * Returns the current section that this block belongs to.
     * @return the current section that this block belongs to.
     */
    State getState() const;


	/**
	* get the absolute path
	* @return return the absolute path
	*/
    char *get_absPath() const;

    /**
     * Sets the current section that this block belongs to the given state.
     * @param state the new section that this block belongs to.
     */
    void setState(State state);

};


#endif //OS_EX4_BLOCK_H
