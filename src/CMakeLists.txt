include_directories(include)
include_directories(${airvent_SOURCE_DIR}/include)
include_directories(${airvent_BINARY_DIR}/include)

set(CMAKE_C_FLAGS "-std=c99")

add_executable(ventd ventd.c commands.c control.c spawn.c)
find_package(argp REQUIRED)

if(ARGP_FOUND)
  include_directories(${ARGP_INCLUDE_DIRS})
  target_link_libraries(ventd ${ARGP_LIBRARIES})
endif(ARGP_FOUND)

target_link_libraries(ventd dl)
target_link_libraries(ventd dht)

configure_file("ventctl.in" "${PROJECT_BINARY_DIR}/ventctl")
configure_file("configure.h.in" "${PROJECT_BINARY_DIR}/include/configure.h")


add_executable(zmqtest zmqtest.c)
find_package(ZeroMQ REQUIRED)
if(ZEROMQ_FOUND)
  include_directories(${ZEROMQ_INCLUDE_DIRS})
  target_link_libraries(zmqtest ${ZEROMQ_LIBRARIES})
endif()

find_package(CZMQ REQUIRED)
if(CZMQ_FOUND)
  include_directories(${CZMQ_INCLUDE_DIRS})
  target_link_libraries(zmqtest ${CZMQ_LIBRARIES})
endif()

install(TARGETS ventd DESTINATION bin)
install(PROGRAMS ${PROJECT_BINARY_DIR}/ventctl DESTINATION bin)
