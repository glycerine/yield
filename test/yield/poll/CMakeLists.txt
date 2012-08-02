cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.poll_test ${CPP})
add_test(yield.poll_test yield.poll_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.poll_test.xml)
target_link_libraries(yield.poll_test yield yield.poll yield.sockets gtest)
