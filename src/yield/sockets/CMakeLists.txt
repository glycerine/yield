cmake_minimum_required(VERSION 2.8)

if (UNIX)
  file(GLOB CPP posix/*.cpp ../../../include/yield/sockets/*.hpp)
elseif (WIN32)
	file(GLOB CPP win32/*.cpp win32/*.hpp ../../../include/yield/sockets/*.hpp)
endif()
add_library(yield.sockets STATIC ${CPP})

set(LIBS yield)
if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
	LIST(APPEND LIBS nsl socket)
endif()
target_link_libraries(yield.sockets ${LIBS})

add_subdirectory(aio)