cmake_minimum_required(VERSION 2.8)

file(GLOB CPP *.cpp ../../../../../include/yield/http/server/ygi/*.hpp)
add_library(yield.http.server.ygi STATIC ${CPP})
target_link_libraries(yield.http.server.ygi yield.http.server)
