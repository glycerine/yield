cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.http_test ${CPP})
target_link_libraries(yield.http_test yield.http gtest)
add_subdirectory(server)