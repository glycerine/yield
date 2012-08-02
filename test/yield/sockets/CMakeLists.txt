cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.sockets_test ${CPP})
target_link_libraries(yield.sockets_test yield.fs yield.sockets gtest)
add_subdirectory(aio)