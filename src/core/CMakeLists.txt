add_library(network OBJECT QtPingerCore.cpp)

target_include_directories(network PUBLIC "${CMAKE_SOURCE_DIR}/network"
  core PUBLIC "${CMAKE_SOURCE_DIR}/core")


