cmake_minimum_required(VERSION 2.8)

file(GLOB CPP "*.cpp")
add_library(yield STATIC ${CPP}) 

# add_subdirectory(fs)
# add_subdirectory(http)
# add_subdirectory(i18n)
# add_subdirectory(poll)
# add_subdirectory(sockets)
# add_subdirectory(stage)
# add_subdirectory(thread)
add_subdirectory(uri)
