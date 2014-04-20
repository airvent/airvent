include_directories(${airvent_SOURCE_DIR}/include)

add_executable(ventd ventd.c)
find_package (argp REQUIRED)

if (ARGP_FOUND)
  include_directories(${ARGP_INCLUDE_DIRS})
  target_link_libraries (ventd ${ARGP_LIBRARIES})
endif(ARGP_FOUND)


configure_file ("ventctl.in" "${PROJECT_BINARY_DIR}/ventctl")
configure_file ("configure.h.in" "${PROJECT_BINARY_DIR}/configure.h")

include_directories("${PROJECT_BINARY_DIR}")

install(TARGETS ventd DESTINATION bin)
install(PROGRAMS ${PROJECT_BINARY_DIR}/ventctl DESTINATION bin)
