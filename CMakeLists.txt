cmake_minimum_required(VERSION 3.0)
project(RPC)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rocket/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rocket/rpc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rocket/net)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rocket/net/tcp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rocket/net/coder)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/conf)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/rocket/common/*.c*)
file(GLOB NET ${CMAKE_CURRENT_SOURCE_DIR}/rocket/net/*.c*)
file(GLOB TCP ${CMAKE_CURRENT_SOURCE_DIR}/rocket/net/tcp/*.c*)
file(GLOB CODER ${CMAKE_CURRENT_SOURCE_DIR}/rocket/net/coder/*.c*)
file(GLOB RPC ${CMAKE_CURRENT_SOURCE_DIR}/rocket/rpc/*.c*)

file(GLOB TEST_server ${CMAKE_CURRENT_SOURCE_DIR}/testcases/test_tcp.cpp)
file(GLOB TEST_client ${CMAKE_CURRENT_SOURCE_DIR}/testcases/test_client.cpp)

add_executable(server ${SRC} ${CODER} ${NET} ${TCP} ${TEST_server} ${RPC})
add_executable(client ${SRC} ${CODER} ${NET} ${TCP} ${TEST_client} ${RPC})

target_link_libraries(server pthread tinyxml protobuf)
target_link_libraries(client pthread tinyxml protobuf)