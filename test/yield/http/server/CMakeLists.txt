cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.http.server_test ${CPP})
target_link_libraries(yield.http.server_test yield.http.server gtest)
add_subdirectory(file)
add_subdirectory(ygi)
