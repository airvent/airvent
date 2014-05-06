cmake_minimum_required(VERSION 2.6)
project(airvent)

set(AIRVENT_VERSION_MAJOR 0)
set(AIRVENT_VERSION_MINOR 0)
set(AIRVENT_VERSION_COMMIT foobar)

# Options
option(BUILD_ZEROMQ "Build our own libzmq and link against it" ON)
option(BUILD_ZEROMQ_TESTS "Build libzmq tests" OFF)
option(BUILD_CZMQ "Build our own czmq and link against it" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(PIPE_DIR "/tmp/")

if(BUILD_ZEROMQ)
  # Instruct cmake to look in built libzmq folder for zmq
  set(ZEROMQ_SEARCH_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/lib/libzmq/lib/Headers/)
  set(ZEROMQ_SEARCH_LIBS ${CMAKE_CURRENT_BINARY_DIR}/lib/libzmq/lib)
endif()

if(BUILD_CZMQ)
  set(CZMQ_SEARCH_INCLUDES ${CMAKE_CURRENT_BINARY_DIR}/lib/include/)
  set(CZMQ_SEARCH_LIBS ${CMAKE_CURRENT_BINARY_DIR}/lib/czmq/)
endif()

set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/lib ${CMAKE_LIBRARY_PATH})

subdirs(lib src)

