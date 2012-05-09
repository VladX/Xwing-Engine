include(config.cmake)
project(${APP_NAME} CXX)
cmake_minimum_required(VERSION 2.6)

configure_file(${CMAKE_SOURCE_DIR}/config.in.lua ${CMAKE_SOURCE_DIR}/config.lua)

add_subdirectory(libs/jemalloc libs/jemalloc)
add_subdirectory(libs/libuv libs/libuv)
add_subdirectory(libs/lua libs/lua)
add_subdirectory(po locale)
add_subdirectory(src objs)
