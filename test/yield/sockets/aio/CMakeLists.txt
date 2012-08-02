cmake_minimum_required(VERSION 2.8)

if (WIN32)
	file(GLOB CPP *.cpp win32/*.cpp)
else()
	file(GLOB CPP *.cpp)
endif()
add_executable(yield.sockets.aio_test ${CPP})
target_link_libraries(yield.sockets.aio_test yield.fs yield.sockets.aio gtest)
