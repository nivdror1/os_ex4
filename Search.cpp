//
// Created by nivdror1 on 5/24/17.
//

#include <fcntl.h>
#include <iostream>
#include <sys/time.h>
#include <algorithm>
#include <vector>
#include <map>
#include "CacheFS.h"

struct Student {
	bool isOld;
};
typedef

int main(){
	std::map<std::pair<int,int>,Student> oldStudentList;
	Student a, b, c, d, e;
	a.isOld = false;
	b.isOld = false;
	c.isOld = true;
	d.isOld = true;
	e.isOld = false;
	std::map<std::pair<int,int>,Student> studentList;
	studentList[std::make_pair(1,3)] = a;
	studentList[std::make_pair(2,3)] = b;
	studentList[std::make_pair(3,3)] = c;
	studentList[std::make_pair(4,3)] = d;
	studentList[std::make_pair(5,3)] = e;
	std::for_each(studentList.begin(), studentList.end(), [&oldStudentList] (const Student& a_s)
	{
		if (a_s.isOld)
		{
			oldStudentList.push_back(a_s);
		}
	});
//	CacheFS_init(5,LRU,0,0);
//	int fd= CacheFS_open("/tmp/niv.txt");
//	void * buf;
//	CacheFS_pread(fd,buf,50,200);
//	CacheFS_pread(fd,buf,50,200);
//	CacheFS_pread(fd,buf,50,300);
//	CacheFS_print_cache("/tmp/niv");
//	CacheFS_print_stat("/tmp/niv");
//	CacheFS_close(fd);
//	CacheFS_destroy();
	return 0;
}
