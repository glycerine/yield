file(GLOB CPP *.cpp *.hpp)
add_executable(yield.thread_test ${CPP})
add_test(yield.thread_test yield.thread_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.thread_test.xml)
target_link_libraries(yield.thread_test yield.thread gtest)
