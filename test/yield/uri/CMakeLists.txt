cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.uri_test ${CPP})
target_link_libraries(yield.uri_test yield.uri gtest)
