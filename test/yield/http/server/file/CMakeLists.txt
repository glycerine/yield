cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.http.server.file_test ${CPP})
target_link_libraries(yield.http.server.file_test yield.http.server.file gtest)
