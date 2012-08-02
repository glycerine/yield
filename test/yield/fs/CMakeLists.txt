cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.fs_test ${CPP})
target_link_libraries(yield.fs_test yield.fs gtest)
add_subdirectory(poll)