cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.poll_test ${CPP})
target_link_libraries(yield.poll_test yield yield.poll yield.sockets gtest)
