project(lua C)
cmake_minimum_required(VERSION 2.6)

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

set(CMAKE_C_FLAGS "")
set(CMAKE_C_FLAGS_RELEASE "")
set(CMAKE_C_FLAGS_DEBUG "")

set(SOURCES
           lapi.c
           lcorolib.c
           ldump.c
           llex.c
           lopcodes.c
           lstrlib.c
           luac.c
           lauxlib.c
           lctype.c
           lfunc.c
           lmathlib.c
           loslib.c
           ltable.c
           lundump.c
           lbaselib.c
           ldblib.c
           lgc.c
           lmem.c
           lparser.c
           ltablib.c
           lvm.c
           lbitlib.c
           ldebug.c
           linit.c
           loadlib.c
           lstate.c
           ltm.c
           lzio.c
           lcode.c
           ldo.c
           liolib.c
           lobject.c
           lstring.c
)

add_definitions(-DLUA_COMPAT_ALL)

if(CMAKE_COMPILER_IS_GNUCC OR CLANG OR MINGW)
 set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -O2")
 set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -O2 -Wall")
endif(CMAKE_COMPILER_IS_GNUCC OR CLANG OR MINGW)

if(MSVC)
 set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD /W1 /O2")
 set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd /W2 /Z7 /RTC1")
endif(MSVC)

if(LINUX)
 add_definitions(-DLUA_USE_LINUX)
 set(LINK_LIBS ${LINK_LIBS} dl)
endif(LINUX)

if(OSX)
 add_definitions(-DLUA_USE_MACOSX)
endif(OSX)

if(BSD)
 add_definitions(-DLUA_USE_LINUX)
endif(BSD)

add_library(lua STATIC ${SOURCES})
target_link_libraries(lua ${LINK_LIBS})
