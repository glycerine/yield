cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp ../../../../../include/yield/http/server/file/*.hpp)
add_library(yield.http.server.file STATIC ${CPP})
target_link_libraries(yield.http.server.file yield.http.server)
