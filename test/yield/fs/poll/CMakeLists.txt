cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.fs.poll_test ${CPP})
target_link_libraries(yield.fs.poll_test yield.fs.poll gtest)
