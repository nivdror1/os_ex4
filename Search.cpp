//
// Created by nivdror1 on 5/24/17.
//

#include <fcntl.h>
#include <iostream>
#include <sys/time.h>
#include "CacheFS.h"

int main(){
	CacheFS_init(5,LRU,0,0);
	int fd= CacheFS_open("+~JF186948981633951426.tmp");
	void * buf;
	CacheFS_pread(fd,buf,500,200);
	CacheFS_close(fd);
	return 0;
}
