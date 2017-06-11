
#ifndef OS_EX4_CACHEFILE_H
#define OS_EX4_CACHEFILE_H


#include <vector>
#include <map>
#include <sys/types.h>

class CacheFile {
private:

	/** the file descriptor*/
	int _fd;

    /** the absolute path of the file*/
	char* absPath;

	/** number of current refernce to the file */
	unsigned int _referenceCount;

public:

	/**
	 * a c-tor
	 * @param fd the file descriptor
	 * @param absolutePath the absolute path of the file
	 * @return an object of CacheFile
	 */
	CacheFile(int fd,char *absolutePath);

	/**
	 * a d-tor
	 */
	~CacheFile();

	/**
	 * get the the file descriptor
	 * @return return the file descriptor
	 */
	int getFd() const;

	/**
	 * get the reference count
	 * @return return the reference count
	 */
    unsigned int getReferenceCount() const;

	/**
	 * get the absolute path
	 * @return return the absolute path
	 */
	char *getAbsPath() const;

	/**
	 * increase the reference count by one
	 */
	void incrementReferenceCount();

	/**
	 * decrease the reference count by one
	 */
    void decrementReferenceCount();

};


#endif //OS_EX4_CACHEFILE_H
