set(LIBRARY_MOZJS_VERSION 33)
set(LIBRARY_MOZJS_NAME mozjs-${LIBRARY_MOZJS_VERSION} PARENT_SCOPE)
set(LIBRARY_MOZJS_NAME mozjs-${LIBRARY_MOZJS_VERSION})
find_library(LIBRARY_MOZJS ${LIBRARY_MOZJS_NAME} ${CMAKE_CURRENT_BINARY_DIR} CMAKE_FIND_ROOT_PATH_BOTH)

if(NOT LIBRARY_MOZJS)
	message(FATAL_ERROR "(!!!) SpiderMonkey v${LIBRARY_MOZJS_VERSION} shared library not found! Please, build it from sources and install to the system library path. Or you can simply place .so/.dll file to the \"${CMAKE_CURRENT_BINARY_DIR}\" and headers to the \"${CMAKE_CURRENT_BINARY_DIR}/include\" instead of installing it to the system directory - cmake will still find it.")
endif(NOT LIBRARY_MOZJS)
