cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.stage_test ${CPP})
target_link_libraries(yield.stage_test yield.stage gtest)
