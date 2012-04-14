project(libuv C)
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
           libuv/src/uv-common.c
           libuv/src/uv-common.h
           libuv/src/ares/ares_cancel.c
           libuv/src/ares/ares__close_sockets.c
           libuv/src/ares/ares_data.c
           libuv/src/ares/ares_data.h
           libuv/src/ares/ares_destroy.c
           libuv/src/ares/ares_dns.h
           libuv/src/ares/ares_expand_name.c
           libuv/src/ares/ares_expand_string.c
           libuv/src/ares/ares_fds.c
           libuv/src/ares/ares_free_hostent.c
           libuv/src/ares/ares_free_string.c
           libuv/src/ares/ares_getenv.h
           libuv/src/ares/ares_gethostbyaddr.c
           libuv/src/ares/ares_gethostbyname.c
           libuv/src/ares/ares__get_hostent.c
           libuv/src/ares/ares_getnameinfo.c
           libuv/src/ares/ares_getopt.c
           libuv/src/ares/ares_getopt.h
           libuv/src/ares/ares_getsock.c
           libuv/src/ares/ares_init.c
           libuv/src/ares/ares_ipv6.h
           libuv/src/ares/ares_library_init.c
           libuv/src/ares/ares_library_init.h
           libuv/src/ares/ares_llist.c
           libuv/src/ares/ares_llist.h
           libuv/src/ares/ares_mkquery.c
           libuv/src/ares/ares_nowarn.c
           libuv/src/ares/ares_nowarn.h
           libuv/src/ares/ares_options.c
           libuv/src/ares/ares_parse_aaaa_reply.c
           libuv/src/ares/ares_parse_a_reply.c
           libuv/src/ares/ares_parse_mx_reply.c
           libuv/src/ares/ares_parse_ns_reply.c
           libuv/src/ares/ares_parse_ptr_reply.c
           libuv/src/ares/ares_parse_srv_reply.c
           libuv/src/ares/ares_parse_txt_reply.c
           libuv/src/ares/ares_platform.h
           libuv/src/ares/ares_private.h
           libuv/src/ares/ares_process.c
           libuv/src/ares/ares_query.c
           libuv/src/ares/ares__read_line.c
           libuv/src/ares/ares_rules.h
           libuv/src/ares/ares_search.c
           libuv/src/ares/ares_send.c
           libuv/src/ares/ares_setup.h
           libuv/src/ares/ares_strcasecmp.c
           libuv/src/ares/ares_strcasecmp.h
           libuv/src/ares/ares_strdup.c
           libuv/src/ares/ares_strdup.h
           libuv/src/ares/ares_strerror.c
           libuv/src/ares/ares_timeout.c
           libuv/src/ares/ares__timeval.c
           libuv/src/ares/ares_version.c
           libuv/src/ares/ares_writev.c
           libuv/src/ares/ares_writev.h
           libuv/src/ares/bitncmp.c
           libuv/src/ares/bitncmp.h
           libuv/src/ares/inet_net_pton.c
           libuv/src/ares/inet_net_pton.h
           libuv/src/ares/inet_ntop.c
           libuv/src/ares/inet_ntop.h
           libuv/src/ares/nameser.h
           libuv/src/ares/setup_once.h
           libuv/src/ares/windows_port.c
)
set(LINK_LIBS "")

add_definitions(-DHAVE_CONFIG_H)

if(CMAKE_COMPILER_IS_GNUCC OR CLANG OR MINGW)
 set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -O2 -w")
 set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -ggdb -Wall")
endif(CMAKE_COMPILER_IS_GNUCC OR CLANG OR MINGW)

if(MSVC)
 set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD /W1 /O2")
 set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd /W2 /Z7 /RTC1")
endif(MSVC)

if(WIN32)
 add_definitions(-D_WIN32_WINNT=0x0600 -DEIO_STACKSIZE=262144 -D_GNU_SOURCE)
 set(LINK_LIBS ${LINK_LIBS} ws2_32 psapi iphlpapi)
 include_directories(libuv/src/ares/config_win32/)
 set(SOURCES ${SOURCES}
             libuv/include/uv-private/tree.h
             libuv/include/uv-private/uv-win.h
             libuv/src/ares/config_win32/ares_config.h
             libuv/src/ares/windows_port.c
             libuv/src/ares/ares_getenv.c
             libuv/src/ares/ares_iphlpapi.h
             libuv/src/ares/ares_platform.c
             libuv/src/win/async.c
             libuv/src/win/cares.c
             libuv/src/win/core.c
             libuv/src/win/dl.c
             libuv/src/win/error.c
             libuv/src/win/fs.c
             libuv/src/win/fs-event.c
             libuv/src/win/getaddrinfo.c
             libuv/src/win/handle.c
             libuv/src/win/internal.h
             libuv/src/win/loop-watcher.c
             libuv/src/win/pipe.c
             libuv/src/win/thread.c
             libuv/src/win/process.c
             libuv/src/win/req.c
             libuv/src/win/stream.c
             libuv/src/win/tcp.c
             libuv/src/win/tty.c
             libuv/src/win/threadpool.c
             libuv/src/win/timer.c
             libuv/src/win/udp.c
             libuv/src/win/util.c
             libuv/src/win/winapi.c
             libuv/src/win/winapi.h
             libuv/src/win/winsock.c
             libuv/src/win/winsock.h
 )
else(WIN32)
 add_definitions(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -DEIO_STACKSIZE=262144)
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
 set(LINK_LIBS ${LINK_LIBS} m)
 include_directories(libuv/src/unix/ev/)
 set(SOURCES ${SOURCES}
             libuv/src/unix/async.c
             libuv/src/unix/cares.c
             libuv/src/unix/check.c
             libuv/src/unix/core.c
             libuv/src/unix/dl.c
             libuv/src/unix/eio/ecb.h
             libuv/src/unix/eio/eio.c
             libuv/src/unix/eio/xthread.h
             libuv/src/unix/error.c
             libuv/src/unix/ev/ev.c
             libuv/src/unix/ev/ev_vars.h
             libuv/src/unix/ev/ev_wrap.h
             libuv/src/unix/ev/event.h
             libuv/src/unix/fs.c
             libuv/src/unix/idle.c
             libuv/src/unix/internal.h
             libuv/src/unix/loop.c
             libuv/src/unix/pipe.c
             libuv/src/unix/prepare.c
             libuv/src/unix/process.c
             libuv/src/unix/stream.c
             libuv/src/unix/tcp.c
             libuv/src/unix/thread.c
             libuv/src/unix/timer.c
             libuv/src/unix/tty.c
             libuv/src/unix/udp.c
             libuv/src/unix/uv-eio.c
             libuv/src/unix/uv-eio.h
 )
endif(WIN32)

if(LINUX)
 include_directories(libuv/src/ares/config_linux/)
 set(SOURCES ${SOURCES}
             libuv/src/unix/linux/core.c
             libuv/src/unix/linux/inotify.c
             libuv/src/unix/linux/syscalls.c
             libuv/src/unix/linux/syscalls.h
 )
 add_definitions(-DEV_CONFIG_H="config_linux.h" -DEIO_CONFIG_H="config_linux.h")
 set(LINK_LIBS ${LINK_LIBS} rt dl)
endif(LINUX)

if(OSX)
 include_directories(libuv/src/ares/config_darwin/)
 set(SOURCES ${SOURCES}
             libuv/src/unix/darwin.c
             libuv/src/unix/kqueue.c
 )
 add_definitions(-D_DARWIN_USE_64_BIT_INODE=1 -DEV_CONFIG_H="config_darwin.h" -DEIO_CONFIG_H="config_darwin.h")
endif(OSX)

if(BSD)
 include_directories(libuv/src/ares/config_freebsd/)
 set(SOURCES ${SOURCES}
             libuv/src/unix/freebsd.c
             libuv/src/unix/kqueue.c
 )
 add_definitions(-DEV_CONFIG_H="config_freebsd.h" -DEIO_CONFIG_H="config_freebsd.h")
 set(LINK_LIBS ${LINK_LIBS} kvm)
endif(BSD)

include_directories(libuv/include/ libuv/include/uv-private/ libuv/src/)
add_library(uv STATIC ${SOURCES})
target_link_libraries(uv ${LINK_LIBS})
