add_library(network OBJECT icmp4.cpp pinger.cpp HostContainer.cpp)

target_include_directories(network PUBLIC "${CMAKE_SOURCE_DIR}/network")
