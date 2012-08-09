file(GLOB CPP *.cpp *.hpp)
add_executable(yield.stage_test ${CPP})
add_test(yield.stage_test yield.stage_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.stage_test.xml)
target_link_libraries(yield.stage_test yield.stage gtest)
