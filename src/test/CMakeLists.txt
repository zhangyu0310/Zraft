cmake_minimum_required(VERSION 2.8.9)

include(../../Zraft_Config.cmake)

# test exec
add_executable(zraft_opt_test
        ../zraft_opt.cc
        zraft_opt_test.cc)
target_link_libraries(zraft_opt_test pthread)
target_link_libraries(zraft_opt_test gtest)
target_link_libraries(zraft_opt_test pthread)

add_executable(json_parse_test
        ../rpc/network.cc
        json_parse_test.cc)
target_link_libraries(json_parse_test pthread)
target_link_libraries(json_parse_test gtest)
target_link_libraries(json_parse_test pthread)
target_link_libraries(json_parse_test bounce)