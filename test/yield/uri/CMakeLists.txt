cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.uri_test ${CPP})
add_test(yield.uri_test yield.uri_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.uri_test.xml)
target_link_libraries(yield.uri_test yield.uri gtest)
