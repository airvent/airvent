add_library(dht dht/dht.c)
install (TARGETS dht DESTINATION lib)
install (FILES dht/dht.h DESTINATION include)

if(BUILD_ZEROMQ)
  set(ZMQ_BUILD_TESTS ${BUILD_ZEROMQ_TESTS})
  add_subdirectory(libzmq)
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libzmq/lib/libzmq.so DESTINATION lib)
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libzmq/lib/Headers/zmq.h DESTINATION include) 
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libzmq/lib/Headers/zmq_utils.h DESTINATION include)
endif()

add_subdirectory(czmq)
add_dependencies(czmq libzmq)
