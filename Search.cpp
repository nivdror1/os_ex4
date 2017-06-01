//
// Created by nivdror1 on 5/24/17.
//

#include <fcntl.h>
#include <iostream>
#include <sys/time.h>
#include "CacheFS.h"

int main(){
	CacheFS_init(5,LRU,0,0);
	int fd= CacheFS_open("/tmp/niv.txt");
	void * buf;
	CacheFS_pread(fd,buf,50,200);
	CacheFS_pread(fd,buf,50,200);
	CacheFS_pread(fd,buf,50,300);
	CacheFS_print_cache("/tmp/niv");
	CacheFS_print_stat("/tmp/niv");
	CacheFS_close(fd);
	CacheFS_destroy();
	return 0;
}
