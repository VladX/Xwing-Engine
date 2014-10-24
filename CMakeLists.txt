include(config.cmake)
project(${APP_NAME} CXX)
cmake_minimum_required(VERSION 2.6)

configure_file(${CMAKE_SOURCE_DIR}/config.in.js ${CMAKE_SOURCE_DIR}/config.js)

add_subdirectory(libs/jemalloc libs/jemalloc)
add_subdirectory(libs/libuv libs/libuv)
add_subdirectory(libs/mozjs libs/mozjs)
add_subdirectory(po locale)
add_subdirectory(src objs)
