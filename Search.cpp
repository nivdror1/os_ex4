//
// Created by nivdror1 on 5/24/17.
//

#include <fcntl.h>
#include <iostream>
int main(){
	off_t off = 100;
	size_t blk = 1024;
	int blockNumber = (int)(off/blk);
//	int fd1= open("/cs/usr/nivdror1/client.py",O_RDONLY);
//	int fd2= open("/cs/usr/nivdror1/client.py",O_RDONLY);
//	std::cout<<"fd1 is "<<fd1<<std::endl;
//	std::cout<<"fd2 is "<<fd2<<std::endl;
	std::cout<<"fd2 is "<<blockNumber<<std::endl;
	return 0;
}
