cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.fs_test ${CPP})
add_test(yield.fs_test yield.fs_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.fs_test.xml)
target_link_libraries(yield.fs_test yield.fs gtest)

add_subdirectory(poll)
