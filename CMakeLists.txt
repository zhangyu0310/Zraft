cmake_minimum_required(VERSION 2.8.9)
project(Zraft)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O0 -Wall")

add_subdirectory(src)
add_subdirectory(src/test)


#add_library(zraft ${ZRAFT_SRC})