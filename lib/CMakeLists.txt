add_library(dht dht/dht.c)
install (TARGETS dht DESTINATION bin)
install (FILES dht/dht.h DESTINATION include)
