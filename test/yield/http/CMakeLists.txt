file(GLOB CPP *.cpp *.hpp)
add_executable(yield.http_test ${CPP})
add_test(yield.http_test yield.http_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.http_test.xml)
target_link_libraries(yield.http_test yield.http gtest)

add_subdirectory(server)
