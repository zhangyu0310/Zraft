cmake_minimum_required(VERSION 2.8.9)

include_directories(
        .
        ../dep
        ../dep/bounce
        ../dep/json/include)

aux_source_directory(. ZRAFT_SRC)
add_library(zraft ${ZRAFT_SRC})