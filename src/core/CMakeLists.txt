# set(CORE_MAIN_SRC QtPingerCore.cpp)
# qt5_wrap_cpp(CORE_MOC_SRC QtPingerCore.hpp)
# add_library(core OBJECT ${CORE_MAIN_SRC} ${CORE_MOC_SRC})

add_library(core OBJECT "QtPingerCore.cpp")

target_link_libraries(core PUBLIC network PUBLIC Qt5::Widgets PUBLIC Qt5::Core)
target_include_directories(core PUBLIC "${CMAKE_SOURCE_DIR}/network"
  PUBLIC "${CMAKE_SOURCE_DIR}/core"
  PUBLIC "${CMAKE_BINARY_DIR}")


