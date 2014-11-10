#### Optional features ####

option(BUILD_INSTALLER "Build installer for windows." OFF)

###########################

include(CheckTypeSize)

check_type_size("void *" TYPE_VOID_POINTER_SIZE BUILTIN_TYPES_ONLY)
math(EXPR COMPILER_TARGET_BITNESS "${TYPE_VOID_POINTER_SIZE} * 8")
message(STATUS "Compilier is ${COMPILER_TARGET_BITNESS}-bit")

if(CMAKE_SYSTEM_NAME MATCHES ".*BSD")
 set(BSD 1)
endif(CMAKE_SYSTEM_NAME MATCHES ".*BSD")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
 set(LINUX 1)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
 set(OSX 1)
endif(CMAKE_SYSTEM_NAME MATCHES "Darwin")

if("${CMAKE_C_COMPILER_ID}" MATCHES "clang" OR "${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
 set(CLANG 1)
endif("${CMAKE_C_COMPILER_ID}" MATCHES "clang" OR "${CMAKE_C_COMPILER_ID}" MATCHES "Clang")

add_subdirectory(test_features test_features)

set(CPPFLAGS "")
set(DEBUG_CPPFLAGS "")
set(RELEASE_CPPFLAGS "")
set(CFLAGS "")
set(DEBUG_CFLAGS "")
set(RELEASE_CFLAGS "")

if(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR CLANG)
 add_definitions(-std=c++0x)
 set(DEBUG_CPPFLAGS "${DEBUG_CPPFLAGS} -Wall -Wextra -ggdb -O0")
 set(RELEASE_CPPFLAGS "${RELEASE_CPPFLAGS} -O3 -g")
 set(CPPFLAGS "${CPPFLAGS} -fno-builtin-malloc -fno-builtin-realloc -fno-builtin-calloc -fno-builtin-free -fno-builtin-memalign")
 #if(COMPILER_HAVE_LTO)
 # set(CPPFLAGS "${CPPFLAGS} -flto")
 #endif(COMPILER_HAVE_LTO)
 #if(COMPILER_HAVE_LINKER_PLUGIN)
 # set(CPPFLAGS "${CPPFLAGS} -fuse-linker-plugin")
 #endif(COMPILER_HAVE_LINKER_PLUGIN)
endif(CMAKE_COMPILER_IS_GNUCXX OR MINGW OR CLANG)

if (MINGW)
 set(CPPFLAGS "${CPPFLAGS} -static-libgcc -static")
endif (MINGW)

if (WIN32)
 add_definitions(-DWINVER=0x0600)
endif (WIN32)

if(MSVC)
 set(CPPFLAGS "${CPPFLAGS} /EHsc")
 set(DEBUG_CPPFLAGS "${DEBUG_CPPFLAGS} /MDd /W3 /Z7 /RTC1")
 set(RELEASE_CPPFLAGS "${RELEASE_CPPFLAGS} /MD /W1 /O2")
endif(MSVC)

if(LINUX OR BSD)
 set(CPPFLAGS "${CPPFLAGS} -pthread")
endif(LINUX OR BSD)

set(DEBUG_LEVEL "0" CACHE STRING "Debug level.")
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_CFLAGS} CACHE STRING "Debug flags." FORCE)
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_CPPFLAGS} CACHE STRING "Debug flags." FORCE)
set(CMAKE_C_FLAGS_RELEASE ${RELEASE_CFLAGS} CACHE STRING "Release flags." FORCE)
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_CPPFLAGS} CACHE STRING "Release flags." FORCE)
set(CMAKE_C_FLAGS ${CFLAGS} CACHE STRING "Flags used by the C compiler during all build types." FORCE)
set(CMAKE_CXX_FLAGS ${CPPFLAGS} CACHE STRING "Flags used by the CXX compiler during all build types." FORCE)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build.")

if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(link_libraries_list ${LIBRARY_MOZJS_NAME} uv lockless)
set(link_directories_list ${CMAKE_BINARY_DIR}/libs/lockless ${CMAKE_BINARY_DIR}/libs/libuv ${CMAKE_BINARY_DIR}/libs/mozjs)
include_directories(${CMAKE_BINARY_DIR}/libs/libuv/libuv/include ${CMAKE_BINARY_DIR}/libs/mozjs/include)
set(exec_filename "${UNIX_PROG_NAME}-server")

file(GLOB source_files *.cpp)
file(GLOB external_source_files ${CMAKE_CURRENT_SOURCE_DIR}/external/*.cpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

link_directories(${link_directories_list})
add_executable(${exec_filename} ${source_files} ${external_source_files})
target_link_libraries(${exec_filename} ${link_libraries_list})
get_property(exec_filename_location TARGET ${exec_filename} PROPERTY LOCATION)
include(CMakeFindBinUtils)
if (CMAKE_BUILD_TYPE MATCHES Release)
 add_custom_command(TARGET ${exec_filename} POST_BUILD COMMAND ${CMAKE_STRIP} ${exec_filename_location})
endif (CMAKE_BUILD_TYPE MATCHES Release)
