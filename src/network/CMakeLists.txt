add_library(network OBJECT pinger.cpp)

target_include_directories(network PUBLIC "${CMAKE_SOURCE_DIR}/network")
