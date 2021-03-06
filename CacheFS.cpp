#include <stdlib.h>
#include <map>
#include "CacheFS.h"
#include "CacheFile.h"
#include "CacheFS.h"
#include "Block.h"
#include "CacheAlgorithm.h"
#include "LRUAlgo.h"
#include "LFUAlgo.h"
#include "FBRAlgo.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <cstring>
#include <ios>
#include <fstream>
#include <iostream>

#define MAX_CHAR_NUMBER 256

/** a map consisting of a file descriptor and a object of the file*/
std::map<int, CacheFile *> openedFiles;

/** a map between the fakeFd and the actual fd*/
std::map<int, int> fakeFDtoFD;

/** the block size given by the os*/
size_t blockSize;

/** the cache algorithm */
CacheAlgorithm* algorithm;

/** the counter of the fake fd*/
int fakeFDCounter;

/**
 * get the directory of the given path
 * @param path the path to the file
 * @return return a string that encapsulates the directory of the path
 */
std::string getDirectory(const char* path){
	std::string temp = path;
	std::size_t found = temp.find_last_of('/');
	return temp.substr(0,found+1); //todo it contain the '/'
}

/**
 * get the file name of the given path
 * @param path the path to the file
 * @return return a string that encapsulates the file name of the path
 */
std::string getFileName(const char* path){
	std::string temp = path;
	std::size_t found = temp.find_last_of('/');
	return temp.substr(found+1,std::string::npos);
}

/**
 * get the absolute path from a soft link
 * @param pathname the original pathname which represent a symbolic link
 * @param resolvedPath the absolute path
 * @return return the absolute path
 */
int getAbsPathFromASoftLink(const char *pathname, char *resolvedPath ){
    // get the absolute path from a symbolic link
    if (readlink(pathname, resolvedPath, 256) == -1) {
        return -1;
    }
    std::string realPath = getDirectory(pathname) + resolvedPath;
    memcpy(resolvedPath, realPath.c_str(),256);
    return 0;
}

/**
 * get the absolute path from a regular file
 * @param pathname the original pathname which represent a relative path
 * @param resolvedPath the absolute path
 * @return return the absolute path
 */
int getAbsPathFromARegularFile(const char *pathname, char *resolvedPath ){
    // get the absolute path from a relative path
    realpath(pathname, resolvedPath);
    if (resolvedPath == nullptr) {
        return -1;
    }
    return 0;
}

/**
 * get the absolute path whether it's a regular file or a symbolic link
 * @param pathname the original pathname - could represent a absolute path/ relative path/ symbolic link
 * @param resolvedPath the absolute path
 * @return return 0 and fill the variable resolvedPath upon succession else return -1
 */
int getAbsolutePath(const char *pathname, char *resolvedPath ){
    struct stat buf;
    int result = 0;
    //check if the pathname is a Symbolic link
    if(lstat(pathname,&buf) != -1 ) {
        if (S_ISLNK(buf.st_mode)) {
            result = getAbsPathFromASoftLink(pathname, resolvedPath);
        }else {
	        result = getAbsPathFromARegularFile(pathname, resolvedPath);
        }
    }
    else{
        result = -1;
    }
    return result;
}

/**
 * check whether if the file is already open
 * @param absPath the absolute path
 * @return on success return the fd else return -1
 */
int isFileCurrentlyOpen(std::string absPath){
	for(auto iter = openedFiles.begin(); iter != openedFiles.end(); ++iter){
		if(absPath == (*iter).second->getAbsPath()){
			return (*iter).first;
		}
	}
	return -1;
}

/**
 * check if the path is a actual file
 * @param path the absolute path
 * @return return whether the path points at a file
 */
int isRegularFile(const char *absPath)
{
	struct stat path_stat;
	if(stat(absPath, &path_stat)!=-1){
		return S_ISREG(path_stat.st_mode);
	}
	return 0;
}

/**
 * check if the path is a actual directory
 * @param path the absolute path
 * @return return whether the path points at a directory
 */
int isDirectory(const char *path) {
	struct stat statbuf;
	if (stat(path, &statbuf) != -1) {
		return S_ISDIR(statbuf.st_mode);
	}
	return 0;

}

/**
 * check whether if the file is already open
 * @param fd the fake file descriptor
 * @return on success return file that match to this fake file descriptor, and NULL if this file
 * is closed
 */
CacheFile* getFileFromFD(int fd){
    auto searchedBlock = fakeFDtoFD.find(fd);
    if(searchedBlock != fakeFDtoFD.end()){
        return openedFiles.at(searchedBlock->second);
    }
    return nullptr;
}


/**
 * check if the path exists
 * @param path the path to the file
 * @param resolvedPath the abs path
 * @param flag true if trying to open file from disc, false if trying to open log file
 * @return return true if the file/directory exists, else return false
 */
bool isPathValid(const char *path, char *resolvedPath, bool flag){
	if(getAbsolutePath(path,resolvedPath) != -1) {
		if (flag && strncmp(resolvedPath, "/tmp/", 5) != 0){
			return false;
		}
		if (isRegularFile(resolvedPath) != 0) {
			return true;
		}
	} else {
        //this is a case of opening a logFile when the file doesn't exists
		std::string dir = getDirectory(path);
		std::string fileName = getFileName(path);
		//check if the directory is valid
		if(isDirectory(dir.c_str()) != 0){
			dir += fileName;
			strcpy(resolvedPath,dir.c_str());
			return true;
		}
	}
	return false;
}
/**
 Initializes the CacheFS.
 Assumptions:
	1. CacheFS_init will be called before any other function.
	2. CacheFS_init might be called multiple times, but only with CacheFS_destroy
  	   between them.

 Parameters:
	blocks_num   - the number of blocks in the buffer cache
	cache_algo   - the cache algorithm that will be used
    f_old        - the percentage of blocks in the old partition (rounding down)
				   relevant in FBR algorithm only
    f_new        - the percentage of blocks in the new partition (rounding down)
				   relevant in FBR algorithm only
 Returned value:
    0 in case of success, negative value in case of failure.
	The function will fail in the following cases:
		1. system call or library function fails (e.g. new).
		2. invalid parameters.
	Invalid parameters are:
		1. blocks_num is invalid if it's not a positive number (zero is invalid too).
		2. f_old is invalid if it is not a number between 0 to 1 or
		   if the size of the partition of the old blocks is not positive.
		3. fNew is invalid if it is not a number between 0 to 1 or
		   if the size of the partition of the new blocks is not positive.
		4. Also, fOld and fNew are invalid if the fOld+fNew is bigger than 1.

		Pay attention: bullets 2-4 are relevant (and should be checked)
		only if cache_algo is FBR.

 For example:
 CacheFS_init(100, FBR, 0.3333, 0.5)
 Initializes a CacheFS that uses FBR to manage the cache.
 The cache contains 100 blocks, 33 blocks in the old partition,
 50 in the new partition, and the remaining 17 are in the middle partition.
 */
int CacheFS_init(int blocks_num, cache_algo_t cache_algo, double f_old , double f_new  ){
    if (blocks_num <= 0){
        return -1;
    }
    fakeFDCounter = 2;

    struct stat st;
    if(stat("/tmp",&st) == -1){
        return -1;
    }
    blockSize = (size_t)st.st_blksize;
    if (cache_algo == cache_algo_t::FBR){
        if (f_new + f_old > 1 || f_new < 0 || f_old < 0 || f_old > 1 || f_new > 1 ){
	        return -1;
        }
	    algorithm = new FBRAlgo(blocks_num, blockSize, f_old, f_new);
    }
	else if(cache_algo==LRU){
		algorithm = new LRUAlgo(blocks_num, blockSize);
	}
	else{
		algorithm = new LFUAlgo(blocks_num, blockSize);
	}

    return 0;
}


/**
 Destroys the CacheFS.
 This function releases all the allocated resources by the library.

 Assumptions:
	1. CacheFS_destroy will be called only after CacheFS_init (one destroy per one init).
	2. After CacheFS_destroy is called,
	   the next CacheFS's function that will be called is CacheFS_init.
	3. CacheFS_destroy is called only after all the open files already closed.
	   In other words, it's the user responsibility to close the files before destroying
	   the CacheFS.

 Returned value:
    0 in case of success, negative value in case of failure.
	The function will fail if a system call or a library function fails.
*/
int CacheFS_destroy(){
    delete algorithm;
    return 0;
}

/**
 * open a new file , adding it to the openedFiles map by creating a new cacheFile object,
 * and adding the fakeFd to the fakeFDtoFD map so it be connected to the real fd
 * @param resolvedPath  the absolute path of the file
 * @return upon success return the fakeFd else return -1
 */
int openANewFile(char * resolvedPath){
    //open the file
    int fd = open(resolvedPath,O_RDONLY | O_DIRECT | O_SYNC);
    if(fd !=-1) {
        //insert the file to the openedFiles map
        try{
            openedFiles[fd] = new CacheFile(fd, resolvedPath);
        }catch(std::bad_alloc e){
            return -1;
        }
        // increase fakeFd counter and add it to the fakeFDtoFD map
        fakeFDCounter++;
        fakeFDtoFD[fakeFDCounter] = fd;
        return fakeFDCounter;
    }
    return -1;
}

/**
 * increase the reference count of the file, increase the fakeFd counter
 * and insert the fakeFD to fakeFDtoFD map
 * @param curFile the real fd
 * @return the fakeFD
 */
int updateAnOldFile(int curFile){
    // the file is already open, return fake FD and add to map FakeFDtoFD
    openedFiles.at(curFile)->incrementReferenceCount();
    fakeFDCounter++;
    fakeFDtoFD[fakeFDCounter] = curFile;
    return fakeFDCounter;
}

/**
 File open operation.
 Receives a path for a file, opens it, and returns an id
 for accessing the file later

 Notes:
	1. You must open the file with the following flags: O_RDONLY | O_DIRECT | O_SYNC
	2. The same file might be opened multiple times.
	   Like in POISX, it's valid.
	3. The pathname is not unique per file, because:
		a. relative paths are not unique: "myFolder/../tmp" and "tmp".
		b. we might open a link ("short-cut") to the file

 Parameters:
    pathname - the path to the file that will be opened

 Returned value:
    In case of success:
		Non negative value represents the id of the file.
		This may be the file descriptor, or any id number that you wish to create.
		This id will be used later to read from the file and to close it.

 	In case of failure:
		Negative number.
		A failure will occur if:
			1. System call or library function fails (e.g. open).
			2. Invalid pathname. Pay attention that we support only files under
			   "/tmp" due to the use of NFS in the Aquarium.
 */
int CacheFS_open(const char *pathname){
    char* resolvedPath= (char *) malloc(sizeof(char) * MAX_CHAR_NUMBER);
    if(resolvedPath == NULL){
        return -1;
    }
	int curFile, finalFd;
	//get the absolute path and check if the file is in tmp directory
    if(isPathValid(pathname, resolvedPath, true)){
	    //check if the file is already open
	    curFile = isFileCurrentlyOpen(resolvedPath);
	    if(curFile == -1){
		    finalFd = openANewFile(resolvedPath);
	    }
        else {
            finalFd = updateAnOldFile(curFile);
        }
    }else{
        finalFd =  -1;
    }
	free(resolvedPath);
	return finalFd;
}


/**
 File close operation.
 Receives id of a file, and closes it.

 Returned value:
	0 in case of success, negative value in case of failure.
	The function will fail in the following cases:
		1. a system call or a library function fails (e.g. close).
		2. invalid file_id. file_id is valid if"f it was returned by
		CacheFS_open, and it is not already closed.
 */
int CacheFS_close(int file_id){
	//check if the file is already open
    CacheFile* curFile = getFileFromFD(file_id);
	if(curFile != nullptr){
        // decrease the reference and erase it from the fakeFDtoFD map
        curFile->decrementReferenceCount();
        fakeFDtoFD.erase(file_id);
        // close the file and delete the CacheFile object
        if (curFile->getReferenceCount() == 0){
            //close the file and remove it from the openedFiles vector
            close(curFile->getFd());
            openedFiles.erase(curFile->getFd());
            delete curFile;
        }
		return 0;
	}
	return -1;
}

/**
 * Converts given offset to number of block in file
 * @param offset the offset to convert
 * @return matching block number
 */
int offsetToBlockNumber(off_t offset){
    if (offset < 0) {
        return -1;
    }
    return (int)(offset/blockSize);
}

/**
 * read from the file/cache block each block at a time until reached the count
 * @param count how many bytes to read
 * @param offset from where to begin reading the file
 * @param fileSize the size of the file
 * @param curFile a CacheFile object
 * @param currentBlockNumber the number of the block in reference to the file
 * @param buf the buffer that will be read from the file
 * @param file_id the file descriptor
 * @return return the number of bytes that was read
 */
int readFromAFile(size_t count, off_t offset, off_t fileSize,
                    CacheFile * curFile, int currentBlockNumber , void* buf , int file_id ){
    int totalBytes = 0;
    int currentBlockBytes = 0;
    struct stat st;
    if(fstat(file_id,&st) != -1) {
        //allocate the size of a block
        void *currentBlockBuffer = aligned_alloc(st.st_blksize, st.st_blksize);
        if (currentBlockBuffer != NULL) {
            //while the count isn't zero the size of the file hasn't been reached
            while (count != 0 && offset + totalBytes != fileSize) {
                //read using of the cache algorithm
                currentBlockBytes = algorithm->read(curFile->getFd(), currentBlockNumber, curFile->getAbsPath(),
                                                    currentBlockBuffer, count, offset + totalBytes);
                if (currentBlockBytes == -1) {
                    return -1;
                }
                //save the block in the buf
                currentBlockNumber++;
                memcpy((char *) buf + totalBytes, currentBlockBuffer, (size_t) currentBlockBytes);
                totalBytes += currentBlockBytes;
                count -= currentBlockBytes;
            }
            free(currentBlockBuffer);
            return totalBytes;
        }
    }
    return -1;

}

/**
   Read data from an open file.

   Read should return exactly the number of bytes requested except
   on EOF or error. For example, if you receive size=100, offset=0,
   but the size of the file is 10, you will initialize only the first
   ten bytes in the buff and return the number 10.

   In order to read the content of a file in CacheFS,
   We decided to implement a function similar to POSIX's pread, with
   the same parameters.

 Returned value:
    In case of success:
		Non negative value represents the number of bytes read.
		See more details above.

 	In case of failure:
		Negative number.
		A failure will occur if:
			1. a system call or a library function fails (e.g. pread).
			2. invalid parameters
				a. file_id is valid if"f it was returned by
			       CacheFS_open, and it wasn't already closed.
				b. buf is invalid if it is NULL.
				c. offset is invalid if it's negative
				   [Note: offset after the end of the file is valid.
				    In this case, you need to return zero,
				    like posix's pread does.]
				[Note: any value of count is valid.]
 */

int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset){
	CacheFile* curFile = getFileFromFD(file_id);
	int currentBlockNumber = offsetToBlockNumber(offset);

	if (curFile != nullptr && buf != NULL && currentBlockNumber >=0){
        // figure out the size of the file
		off_t fileSize = lseek(curFile->getFd(),0,SEEK_END);
        //if the size of the file is smaller than the offset than there is nothing to read
		if(fileSize< offset ){
			return 0;
		}else if(fileSize != -1) {
            //read from the file
            return readFromAFile(count, offset, fileSize, curFile, currentBlockNumber, buf,
                                 curFile->getFd());
        }
	}
	return -1;
}

/**
 * write to the log file all of the blocks that in the cache by the order of the cache algorithm dictates
 * in each block shall be printed the follows: the path of the file and the number of the block
 * @param resolvedPath the log file path
 * @return upon success return 0 , else return -1
 */
int printCacheToTheLog( char* resolvedPath ){
    //create the ofstream
    std::ofstream logFile;
    logFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        //open the log file in order to be written
        logFile.open(resolvedPath, std::ofstream::app);
        // receiving a vector that contains the absPath and
        // the number of the block by the order of the algorithm
        auto cacheBlocks = algorithm->getOrderedCache();
        cacheBlocks.reverse();
        //print the cache info
        for (auto iter = cacheBlocks.begin(); iter != cacheBlocks.end(); ++iter) {
            char *path = algorithm->getBlockFromCache((*iter).first, (*iter).second)->get_absPath();
            logFile << path << " " << (*iter).second
                    << std::endl;
        }
        logFile.close();
        return 0;
    } catch (std::ofstream::failure e) {
        return -1;
    }
}

/**
 * write to the log file the number of hits and the number of misses
 * @param resolvedPath the log file path
 * @return upon success return 0 , else return -1
 */
int printStatsToTheLog( char* resolvedPath ){
    //create the ofstream
    std::ofstream logFile;
    logFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        //open the log file in order to be written
        logFile.open(resolvedPath, std::ofstream::app);

        logFile<< "Hits number: "<< algorithm->getNumberOfHits()<<std::endl;
        logFile<< "Misses number: " << algorithm->getNumberOfMisses()<<std::endl;

        logFile.close();
        return 0;
    } catch (std::ofstream::failure e) {
        return -1;
    }
}

/**
This function writes the current state of the cache to a file.
The function writes a line for every block that was used in the cache
(meaning, each block with at least one access).
Each line contains the following values separated by a single space.
	1. Full path of the file
	2. The number of the block. Pay attention: this is not the number in the cache,
	   but the enumeration within the file itself, starting with 0 for the first
	   block in each file.
For LRU and LFU The order of the entries is from the last block that will be evicted from the cache
to the first (next) block that will be evicted.
For FBR use the LRU order (the order of the stack).

Notes:
	1. If log_path is a path to existed file - the function will append the cache
	   state (described above) to the cache.
	   Otherwise, if the path is valid, but the file doesn't exist -
	   a new file will be created.
	   For example, if "/tmp" contains a single folder named "folder", then
			"/tmp/folder/myLog" is valid, while "/tmp/not_a_folder/myLog" is invalid.
	2. Of course, this operation doesn't change the cache at all.
	3. log_path doesn't have to be under "/tmp".
	3. This function might be useful for debugging purpose as well as auto-tests.
	   Make sure to follow the syntax and order as explained above.

 Parameter:
	log_path - a path of the log file. A valid path is either: a path to an existing
			   log file or a path to a new file (under existing directory).

 Returned value:
    0 in case of success, negative value in case of failure.
	The function will fail in the following cases:
		1. system call or library function fails (e.g. open, write).
		2. log_path is invalid.
 */
int CacheFS_print_cache (const char *log_path){
	int result = 0;
	char* resolvedPath= (char *) malloc(sizeof(char) * MAX_CHAR_NUMBER);
    if (resolvedPath!= NULL){
        if(isPathValid(log_path, resolvedPath, false)) {
            result = printCacheToTheLog( resolvedPath);
        }else{
            result  = -1;
        }
        free(resolvedPath);
        return result;
    }else{
        return -1;
    }


}


/**
This function writes the statistics of the CacheFS to a file.
This function writes exactly the following lines:
Hits number: HITS_NUM.
Misses number: MISS_NUM.

Where HITS_NUM is the number of cache-hits, and MISS_NUM is the number of cache-misses.
A cache miss counts the number of fetched blocks from the disk.
A cache hit counts the number of required blocks that were already stored
in the cache (and therefore we didn't fetch them from the disk again).

Notes:
	1. If log_path is a path to existed file - the function will append the cache
	   state (described above) to the cache.
	   Otherwise, if the path is valid, but the file doesn't exist -
	   a new file will be created.
	   For example, if "/tmp" contains a single folder named "folder", then
			"/tmp/folder/myLog" is valid, while "/tmp/not_a_folder/myLog" is invalid.
	2. Of course, this operation doesn't change the cache at all.
	3. log_path doesn't have to be under "/tmp".

 Parameter:
	log_path - a path of the log file. A valid path is either: a path to an existing
			   log file or a path to a new file (under existing directory).

 Returned value:
    0 in case of success, negative value in case of failure.
	The function will fail in the following cases:
		1. system call or library function fails (e.g. open, write).
		2. log_path is invalid.
 */
int CacheFS_print_stat (const char *log_path){
	int result = 0;
	char* resolvedPath= (char *) malloc(sizeof(char) * MAX_CHAR_NUMBER);
    if(resolvedPath !=NULL){
        if(isPathValid(log_path, resolvedPath, false)) {
            result = printStatsToTheLog(resolvedPath);
        }else{
            result = -1;
        }
        free(resolvedPath);
        return result;
    }
	else{
        return -1;
    }
}
