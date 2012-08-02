cmake_minimum_required(VERSION 2.8)

project(yield CXX)

if (MSVC)
	add_definitions("/GR- /MP /wd\"4100\" /wd\"4127\" /wd\"4290\" /wd\"4355\" /wd\"4512\"")
else()
	add_definitions(-fno-rtti -Wall -Wextra -Wfloat-equal -Winit-self -Winline -Wold-style-cast -Wunused)
endif()

include_directories(include)

set(CMAKE_USE_RELATIVE_PATHS TRUE FORCE)

add_subdirectory(src)
add_subdirectory(test)
