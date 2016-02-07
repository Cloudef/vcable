configure_file(config.h.in config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(vcable STATIC vcable.c)
add_subdirectory(plugins)
