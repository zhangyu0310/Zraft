cmake_minimum_required(VERSION 2.8.9)
include(../Zraft_Config.cmake)

aux_source_directory(. ZRAFT_SRC)
aux_source_directory(rpc ZRAFT_RPC)
add_library(zraft
        ${ZRAFT_SRC}
        ${ZRAFT_RPC})