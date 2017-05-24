cmake_minimum_required(VERSION 3.6)
project(os_ex4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")

set(SOURCE_FILES Search.cpp CacheFile.cpp Block.cpp CacheFS.cpp)
add_executable(os_ex4 ${SOURCE_FILES})