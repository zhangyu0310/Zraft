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

add_executable(rpc_test_ser
        rpc_test_ser.cc
        ../rpc/rpc_server.cc
        ../rpc/rpc.cc)
target_link_libraries(rpc_test_ser pthread)
target_link_libraries(rpc_test_ser bounce)

add_executable(rpc_test_cli
        rpc_test_cli.cc
        ../rpc/rpc_server.cc
        ../rpc/rpc.cc)
target_link_libraries(rpc_test_cli pthread)
target_link_libraries(rpc_test_cli bounce)

add_executable(zraft_connections_test
        zraft_connections_test.cc)
target_link_libraries(zraft_connections_test pthread)
target_link_libraries(zraft_connections_test bounce)
target_link_libraries(zraft_connections_test zraft)
target_link_libraries(zraft_connections_test bounce)

add_executable(zraft_election_test
        zraft_election_test.cpp)
target_link_libraries(zraft_election_test pthread)
target_link_libraries(zraft_election_test bounce)
target_link_libraries(zraft_election_test zraft)
target_link_libraries(zraft_election_test bounce)