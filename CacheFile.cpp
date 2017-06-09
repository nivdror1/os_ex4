

#include <cstring>
#include <malloc.h>
#include "CacheFile.h"


/**
 * a c-tor
 * @param fd the file descriptor
 * @param absolutePath the absolute path of the file
 * @return an object of CacheFile
 */
CacheFile::CacheFile(int fd, char *absolutePath):_fd(fd), _referenceCount(1){
	this->absPath = (char*)malloc(strlen(absolutePath)+1);
	memcpy(absPath,absolutePath, strlen(absolutePath)+1);
}

/**
 * a d-tor
 */
CacheFile::~CacheFile(){
	free(absPath);
}

/**
 * get the the file descriptor
 * @return return the file descriptor
 */
int CacheFile::getFd() const{
	return _fd;
}

/**
 * get the reference count
 * @return return the reference count
 */
unsigned int CacheFile::getReferenceCount() const
{
    return _referenceCount;
}

/**
 * increase the reference count by one
 */
void CacheFile::incrementReferenceCount()
{
    CacheFile::_referenceCount += 1;
}

/**
 * decrease the reference count by one
 */
void CacheFile::decrementReferenceCount()
{
	CacheFile::_referenceCount -= 1;
}

/**
 * get the absolute path
 * @return return the absolute path
 */
char *CacheFile::getAbsPath() const
{
	return absPath;
}
