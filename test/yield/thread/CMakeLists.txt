cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.thread_test ${CPP})
target_link_libraries(yield.thread_test yield.thread gtest)
