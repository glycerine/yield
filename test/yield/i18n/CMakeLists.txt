cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp *.hpp)
add_executable(yield.i18n_test ${CPP})
target_link_libraries(yield.i18n_test yield.i18n gtest)
