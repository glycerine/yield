file(GLOB CPP *.cpp *.hpp)
add_executable(yield.http.server.ygi_test ${CPP})
add_test(yield.http.server.ygi_test yield.http.server.ygi_test --gtest_output=xml:${CMAKE_BINARY_DIR}/gtest_output/yield.http.server.ygi_test.xml)
target_link_libraries(yield.http.server.ygi_test yield.http.server.ygi gtest)
