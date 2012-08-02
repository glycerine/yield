cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp ../../../../include/yield/http/server/*.hpp)
add_library(yield.http.server STATIC ${CPP})
target_link_libraries(yield.http.server yield.fs yield.http yield.sockets.aio yield.stage)
add_subdirectory(file)
add_subdirectory(ygi)
