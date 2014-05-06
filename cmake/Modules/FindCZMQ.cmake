if (BUILD_CZMQ)
  message(STATUS "looking for CZMQ in ${CZMQ_SEARCH_INCLUDES}; ${CZMQ_SEARCH_LIBS}")
  set(CZMQ_INCLUDE_DIRS ${CZMQ_SEARCH_INCLUDES})
  set(CZMQ_LIBRARIES ${CZMQ_SEARCH_LIBS}/libczmq.so)

else ()
  include(FindPkgConfig)
  PKG_CHECK_MODULES(PC_CZMQ "libczmq")

  find_path(
    CZMQ_INCLUDE_DIRS
    NAMES zmq.h zmq_utils.h
    HINTS ${PC_CZMQ_INCLUDE_DIRS}
  )
  find_library(
    CZMQ_LIBRARIES
    NAMES zmq
    HINTS ${PC_CZMQ_LIBRARY_DIRS}
  )
endif ()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CZMQ DEFAULT_MSG CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS)
mark_as_advanced(CZMQ_LIBRARIES CZMQ_INCLUDE_DIRS)
