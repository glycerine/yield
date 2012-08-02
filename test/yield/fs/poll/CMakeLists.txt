cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.fs.poll_test ${CPP})
add_test(yield.fs.poll_test yield.fs.poll_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.fs.poll_test.xml)
target_link_libraries(yield.fs.poll_test yield.fs.poll gtest)
