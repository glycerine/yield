file(GLOB CPP *.cpp *.rl ../../../include/yield/uri/*.hpp)
add_library(yield.uri STATIC ${CPP})
target_link_libraries(yield.uri yield)