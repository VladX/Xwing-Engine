include(config.cmake)
project(${APP_NAME} CXX)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(libs/jemalloc libs/jemalloc)
add_subdirectory(libs/libuv libs/libuv)
add_subdirectory(po locale)
add_subdirectory(src objs)
