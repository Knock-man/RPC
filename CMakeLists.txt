cmake_minimum_required(VERSION 3.0)
project(RPC)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the build type." FORCE)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rocket/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rocket/net)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/conf)

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/rocket/common/*.c*)
file(GLOB NET ${CMAKE_CURRENT_SOURCE_DIR}/rocket/net/*.c*)
file(GLOB TEST ${CMAKE_CURRENT_SOURCE_DIR}/testcases/*.c*)

add_executable(app ${SRC} ${NET} ${TEST})

target_link_libraries(app pthread tinyxml)