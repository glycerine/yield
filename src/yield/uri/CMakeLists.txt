cmake_minimum_required(VERSION 2.8)

file(GLOB CPP "*.cpp")
add_library(yield.uri STATIC ${CPP})
add_dependencies(yield.uri yield)