file(GLOB CPP *.cpp ../../../include/yield/stage/*.hpp)
add_library(yield.stage STATIC ${CPP})
target_link_libraries(yield.stage yield.thread)
