cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.i18n_test ${CPP})
add_test(yield.i18n_test yield.i18n_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.i18n_test.xml)
target_link_libraries(yield.i18n_test yield.i18n gtest)
