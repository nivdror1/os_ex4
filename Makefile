CPP= g++ -pthread
CPPFLAGS= -c -g -Wextra -Wvla -Wall -std=c++11 -DNDEBUG
TAR_FILES_PART1 = Makefile README CacheFS.cpp Block.h Block.cpp CacheAlgorithm.h CacheAlgorithm.cpp
TAR_FILES_PART2 = LRUAlgo.h LRUAlgo.cpp LFUAlgo.h LFUAlgo.cpp FBRAlgo.h FBRAlgo.cpp CacheFile.h CacheFile.cpp
TAR_FILES= $(TAR_FILES_PART1) $(TAR_FILES_PART2)

# All Target
all: CacheFS

#Library
CacheFS: CacheFS.o CacheAlgorithm.o LFUAlgo.o LRUAlgo.o FBRAlgo.o Block.o CacheFile.o
	ar rcs CacheFS.a CacheFS.o CacheAlgorithm.o LFUAlgo.o LRUAlgo.o FBRAlgo.o Block.o CacheFile.o
	

# Object Files
	
CacheFS.o: CacheAlgorithm.h LFUAlgo.h LRUAlgo.h FBRAlgo.h CacheFile.h CacheFS.cpp
	$(CPP) $(CPPFLAGS) CacheFS.cpp -o CacheFS.o
	
LFUAlgo.o: LFUAlgo.h LFUAlgo.cpp CacheAlgorithm.h
	$(CPP) $(CPPFLAGS) LFUAlgo.cpp -o LFUAlgo.o
	
LRUAlgo.o: LRUAlgo.h LRUAlgo.cpp CacheAlgorithm.h
	$(CPP) $(CPPFLAGS) LRUAlgo.cpp -o LRUAlgo.o

FBRAlgo.o: FBRAlgo.h FBRAlgo.cpp CacheAlgorithm.h
	$(CPP) $(CPPFLAGS) FBRAlgo.cpp  -o FBRAlgo.o
	
CacheAlgorithm.o: CacheAlgorithm.h CacheAlgorithm.cpp Block.h
	$(CPP) $(CPPFLAGS) CacheAlgorithm.cpp  -o CacheAlgorithm.o
	
CacheFile.o: CacheFile.h CacheFile.cpp
	$(CPP) $(CPPFLAGS) CacheFile.cpp  -o CacheFile.o

Block.o: Block.h Block.cpp
	$(CPP) $(CPPFLAGS) Block.cpp  -o Block.o

tar:
	tar cvf ex4.tar $(TAR_FILES)
 
# Other Targets
clean:
	-rm -f *.o CacheFS.a ex4.tar
