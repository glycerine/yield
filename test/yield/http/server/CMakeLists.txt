file(GLOB CPP *.cpp *.hpp)
add_executable(yield.http.server_test ${CPP})
# add_test(yield.http.server_test yield.http.server_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.http.server_test.xml)
target_link_libraries(yield.http.server_test yield.http.server gtest)

add_subdirectory(file)
add_subdirectory(ygi)
