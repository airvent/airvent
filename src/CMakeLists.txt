include_directories(include)
include_directories(${airvent_SOURCE_DIR}/include)
include_directories(${airvent_BINARY_DIR}/include)

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


install(TARGETS ventd DESTINATION bin)
install(PROGRAMS ${PROJECT_BINARY_DIR}/ventctl DESTINATION bin)
