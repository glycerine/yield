cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.queue_test ${CPP})
add_dependencies(yield.queue_test yield.queue)
target_link_libraries(yield.queue_test yield.thread gtest)
