add_library(core OBJECT QtPingerCore.cpp)

target_link_libraries(core PUBLIC network PUBLIC Qt5::Widgets PUBLIC Qt5::Core)
target_include_directories(core PUBLIC "${CMAKE_SOURCE_DIR}/network"
  PUBLIC "${CMAKE_SOURCE_DIR}/core")


