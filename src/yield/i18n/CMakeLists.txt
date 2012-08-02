cmake_minimum_required(VERSION 2.8)

if (UNIX)
  file(GLOB CPP posix/*.cpp ../../../include/yield/i18n/*.hpp)
elseif (WIN32)
	file(GLOB CPP win32/*.cpp ../../../include/yield/i18n/*.hpp)
endif()
add_library(yield.i18n STATIC ${CPP})
target_link_libraries(yield.i18n yield)
