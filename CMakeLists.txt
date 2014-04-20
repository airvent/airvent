cmake_minimum_required (VERSION 2.6)
project (airvent)

set(AIRVENT_VERSION_MAJOR 0)
set(AIRVENT_VERSION_MINOR 0)
set(AIRVENT_VERSION_COMMIT foobar)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


set(PIPE_DIR "/tmp/")

subdirs(src)

