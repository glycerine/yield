cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.c *.cpp *.h)
add_executable(yield.http.server.ygi_test ${CPP})
set_source_files_properties(test_ygi_request_handler.c PROPERTIES LANGUAGE C)
target_link_libraries(yield.http.server.ygi_test yield.http.server.ygi gtest)
