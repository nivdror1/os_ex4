#include <stdlib.h>
#include <map>
#include "CacheFS.h"
#include "CacheFile.h"
#include "CacheFS.h"
#include "Block.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>


std::vector<std::pair<int, std::string> >openedFiles;

void* copyBuffer;

size_t blockSize;

/**
 * get the absolute path whether it's a regular file or a symbolic link
 * @param pathname the original pathname - could represent a absolute path/ relative path/ symbolic link
 * @param resolvedPath the absolute path
 * @return return 0 and fill the variable resolvedPath upon succession else return -1
 */
static int getAbsolutePath(const char *pathname,char *resolvedPath ){
    struct stat buf;
    int result = 0;
    //check if the pathname is a Symbolic link
    if(lstat(pathname,&buf)!=-1) {
        if (S_ISLNK(buf.st_mode)) {
            // get the absolute path from a symbolic link
            if (readlink(pathname, resolvedPath, 256) == -1) {
                result--;
            }
        } else {
            //todo do i need to find out whether this is a regular file
            // get the absolute path from a relative path
            realpath(pathname, resolvedPath);
            if (resolvedPath == nullptr) {
                result--;
            }
        }
    }
    else{
        result--;
    }
    return result;
}

/**
 * check whether if the file is allready open
 * @param absPath the absolute path
 * @return on success return the fd else return -1
 */
int isFileCurrentlyOpen(std::string absPath){
	for(unsigned int i=0;i<openedFiles.size();i++){
		if(absPath==openedFiles.at(i).second){
			return openedFiles.at(i).first;
		}
	}
	return -1;
}

/**
 * check whether if the file is allready open
 * @param fd the file descriptor
 * @return on success return the location of the fd in the opened files vector, else return -1
 */
int isFileCurrentlyOpen(int fd){
	for(unsigned int i=0;i<openedFiles.size();i++){
		if(fd==openedFiles.at(i).first){
			return i;
		}
	}
	return -1;
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
int CacheFS_init(int blocks_num, cache_algo_t cache_algo,
                 double f_old , double f_new  ){
    if (blocks_num <= 0){
        // todo error
        return -1;
    }
    if (cache_algo == cache_algo_t::FBR){
        if (f_new + f_old > 1 || f_new < 0 || f_old < 0 || f_old > 1 || f_new > 1 ){
            // todo error
        }
        return -1;
    }
    struct stat fi;
    stat("/tmp", &fi);
    blockSize = (size_t)fi.st_blksize;
    if ((copyBuffer = malloc(blockSize)) == NULL){
        //todo error
        return -1;
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
    free(copyBuffer);
    //todo delete the algorithm
    return 0;
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
    char *resolvedPath= nullptr;
	int fd,curFile;
    //todo check if the pathname is \tmp something
	//get the absolute path
    if(getAbsolutePath(pathname,resolvedPath)){
	    //check if the file is already open
	    curFile = isFileCurrentlyOpen(resolvedPath);
	    if(curFile == -1){
		    //open the file and append the fd and the absPath to the openedFiles vector
		    fd = open(resolvedPath,O_RDONLY | O_DIRECT | O_SYNC);
		    if(fd !=-1) {
			    openedFiles.push_back(std::make_pair(fd, resolvedPath));
		    }
		    return fd;
	    }
	    return curFile;
    }else{
        return -1;
    }
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
    int curFile = isFileCurrentlyOpen(file_id);
	if(curFile!=-1){
		//close the file and remove it from the openedFiles vector
		close(file_id);
		openedFiles.erase(openedFiles.begin()+curFile);
		return 0;
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
int CacheFS_pread(int file_id, void *buf, size_t count, off_t offset);

/**
This function writes the current state of the cache to a file.
The function writes a line for every block that was used in the cache
(meaning, each block with at least one access).
Each line contains the following values separated by a single space.
	1. Full path of the file
	2. The number of the block. Pay attention: this is not the number in the cache,
	   but the enumeration within the file itself, starting with 0 for the first
	   block in each file.
The order of the entries is from the last block that will be evicted from the cache
to the first (next) block that will be evicted.

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
int CacheFS_print_cache (const char *log_path);


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
int CacheFS_print_stat (const char *log_path);
