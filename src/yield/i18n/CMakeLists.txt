if (UNIX)
  file(GLOB CPP posix/*.cpp ../../../include/yield/i18n/*.hpp)
elseif (WIN32)
	file(GLOB CPP win32/*.cpp ../../../include/yield/i18n/*.hpp)
endif()
add_library(yield.i18n STATIC ${CPP})

set(LIBS yield)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	LIST(APPEND LIBS iconv)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
	LIST(APPEND LIBS iconv intl)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
	LIST(APPEND LIBS iconv)
endif()
target_link_libraries(yield.i18n ${LIBS})
