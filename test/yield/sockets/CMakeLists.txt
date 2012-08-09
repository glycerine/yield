file(GLOB CPP *.cpp *.hpp)
add_executable(yield.sockets_test ${CPP})
add_test(yield.sockets_test yield.sockets_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.sockets_test.xml)
target_link_libraries(yield.sockets_test yield.fs yield.sockets gtest)

add_subdirectory(aio)
