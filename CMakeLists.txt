cmake_minimum_required(VERSION 3.14)
project(RingSketch)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O2 -g -mavx2")

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories(.)
include_directories(Common)
include_directories(Struct)
include_directories(Algos)
include_directories(Bench)

find_package(Threads REQUIRED)

add_executable(comparison comparison.cpp)
target_link_libraries(comparison Threads::Threads)

add_executable(parameter parameter.cpp)
target_link_libraries(parameter Threads::Threads)

add_executable(batch batch.cpp)
target_link_libraries(batch Threads::Threads)

add_executable(periodic periodic.cpp)
target_link_libraries(periodic Threads::Threads)

add_executable(cardinality cardinality.cpp)
target_link_libraries(cardinality Threads::Threads)

add_executable(math math.cpp)
target_link_libraries(math Threads::Threads)

add_executable(freshness freshness.cpp)
target_link_libraries(freshness Threads::Threads)
