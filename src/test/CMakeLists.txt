cmake_minimum_required(VERSION 2.8.9)

include_directories(..)

# test exec
add_executable(zraft_opt_test
        ../zraft_opt.cc
        zraft_opt_test.cc)
target_link_libraries(zraft_opt_test pthread)
target_link_libraries(zraft_opt_test gtest)
target_link_libraries(zraft_opt_test pthread)