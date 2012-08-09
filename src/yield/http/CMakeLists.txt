file(GLOB CPP *.cpp *.rl ../../../include/yield/http/*.hpp)
add_library(yield.http STATIC ${CPP})
target_link_libraries(yield.http yield.uri)
add_subdirectory(server)