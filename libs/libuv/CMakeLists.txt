project(libuv C)
cmake_minimum_required(VERSION 2.6)
include(CheckCSourceCompiles)

if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

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
                   libuv/src/fs-poll.c
                   libuv/src/heap-inl.h
                   libuv/src/inet.c
                   libuv/src/queue.h
                   libuv/src/threadpool.c
                   libuv/src/uv-common.c
                   libuv/src/uv-common.h
                   libuv/src/version.c)
set(LINK_LIBS "")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libuv/include ${CMAKE_CURRENT_SOURCE_DIR}/libuv/src)

set(CMAKE_REQUIRED_FLAGS "-flto")
check_c_source_compiles("int main (void) { return 0; }" COMPILER_HAVE_LTO)
set(CMAKE_REQUIRED_FLAGS "-fuse-linker-plugin")
check_c_source_compiles("int main (void) { return 0; }" COMPILER_HAVE_LINKER_PLUGIN)
set(CMAKE_REQUIRED_FLAGS "")

if(CMAKE_COMPILER_IS_GNUCC OR CLANG OR MINGW)
 set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -O2 -w")
 set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -Wall")
 if(COMPILER_HAVE_LTO)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")
 endif(COMPILER_HAVE_LTO)
 if(COMPILER_HAVE_LINKER_PLUGIN)
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fuse-linker-plugin")
 endif(COMPILER_HAVE_LINKER_PLUGIN)
endif(CMAKE_COMPILER_IS_GNUCC OR CLANG OR MINGW)

if(MSVC)
 set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD /W1 /O2")
 set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd /W2 /Z7 /RTC1")
endif(MSVC)

if(WIN32)
 add_definitions(-D_WIN32_WINNT=0x0600 -DWIN32_LEAN_AND_MEAN)
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libuv/src/win)
 set(LINK_LIBS ${LINK_LIBS} ws2_32 psapi iphlpapi shell32)
 set(SOURCES ${SOURCES}
                    libuv/src/win/async.c
                    libuv/src/win/atomicops-inl.h
                    libuv/src/win/core.c
                    libuv/src/win/dl.c
                    libuv/src/win/error.c
                    libuv/src/win/fs-event.c
                    libuv/src/win/fs.c
                    libuv/src/win/getaddrinfo.c
                    libuv/src/win/getnameinfo.c
                    libuv/src/win/handle.c
                    libuv/src/win/handle-inl.h
                    libuv/src/win/internal.h
                    libuv/src/win/loop-watcher.c
                    libuv/src/win/pipe.c
                    libuv/src/win/poll.c
                    libuv/src/win/process-stdio.c
                    libuv/src/win/process.c
                    libuv/src/win/req.c
                    libuv/src/win/req-inl.h
                    libuv/src/win/signal.c
                    libuv/src/win/stream.c
                    libuv/src/win/stream-inl.h
                    libuv/src/win/tcp.c
                    libuv/src/win/thread.c
                    libuv/src/win/timer.c
                    libuv/src/win/tty.c
                    libuv/src/win/udp.c
                    libuv/src/win/util.c
                    libuv/src/win/winapi.c
                    libuv/src/win/winapi.h
                    libuv/src/win/winsock.c
                    libuv/src/win/winsock.h)
else(WIN32)
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=gnu89 -Wno-unused-parameter")
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libuv/src/unix)
 set(SOURCES ${SOURCES}
                   libuv/src/unix/async.c
                   libuv/src/unix/atomic-ops.h
                   libuv/src/unix/core.c
                   libuv/src/unix/dl.c
                   libuv/src/unix/fs.c
                   libuv/src/unix/getaddrinfo.c
                   libuv/src/unix/getnameinfo.c
                   libuv/src/unix/internal.h
                   libuv/src/unix/loop-watcher.c
                   libuv/src/unix/loop.c
                   libuv/src/unix/pipe.c
                   libuv/src/unix/poll.c
                   libuv/src/unix/process.c
                   libuv/src/unix/signal.c
                   libuv/src/unix/spinlock.h
                   libuv/src/unix/stream.c
                   libuv/src/unix/tcp.c
                   libuv/src/unix/thread.c
                   libuv/src/unix/timer.c
                   libuv/src/unix/tty.c
                   libuv/src/unix/udp.c)
endif(WIN32)

if(LINUX)
 set(SOURCES ${SOURCES}
                    libuv/src/unix/linux-core.c
                    libuv/src/unix/linux-inotify.c
                    libuv/src/unix/linux-syscalls.c
                    libuv/src/unix/linux-syscalls.h
                    libuv/src/unix/proctitle.c)
endif(LINUX)

if(OSX)
 set(SOURCES ${SOURCES}
                    libuv/src/unix/darwin.c
                    libuv/src/unix/darwin-proctitle.c
                    libuv/src/unix/fsevents.c
                    libuv/src/unix/kqueue.c
                    libuv/src/unix/proctitle.c)
 add_definitions(-D_DARWIN_USE_64_BIT_INODE=1 -D_DARWIN_UNLIMITED_SELECT=1)
endif(OSX)

if(BSD)
 set(SOURCES ${SOURCES} libuv/src/unix/freebsd.c libuv/src/unix/kqueue.c)
endif(BSD)

include_directories(libuv/include/ libuv/include/uv-private/ libuv/src/)
add_library(uv STATIC ${SOURCES})
target_link_libraries(uv ${LINK_LIBS})
