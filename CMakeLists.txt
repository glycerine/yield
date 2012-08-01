cmake_minimum_required(VERSION 2.8)

project(yield CXX)

include_directories(include)

set(CMAKE_USE_RELATIVE_PATHS, TRUE)

add_subdirectory(src)
add_subdirectory(test)