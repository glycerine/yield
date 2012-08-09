file(GLOB HPP ../../../include/yield/queue/*.hpp)
add_custom_target(yield.queue SOURCES ${HPP})
add_dependencies(yield.queue yield.thread)