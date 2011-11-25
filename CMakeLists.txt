cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME_STR af_concurrent)
project(${PROJECT_NAME_STR})

add_definitions(-g -Wall -Wextra -pthread)

set(CMAKE_CXX_FLAGS "-std=c++0x")
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)

find_package(Boost REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(src_test)
