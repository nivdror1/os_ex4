cmake_minimum_required(VERSION 3.6)
project(os_ex4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wextra -Wall ")

set(SOURCE_FILES FBRAlgo.cpp CacheFS.cpp CacheAlgorithm.cpp Block.cpp LFUAlgo.cpp
        LRUAlgo.cpp CacheFile.cpp TEST.cpp)
add_executable(os_ex4 ${SOURCE_FILES})