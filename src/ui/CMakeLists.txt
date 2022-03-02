set(UISOURCES pinger_main_window.cpp)

qt5_wrap_ui(UISOURCES designer/QtPingUI_1.ui)
qt5_wrap_cpp(UISOURCES pinger_main_window.hpp)
add_library(ui OBJECT ${UISOURCES})

target_link_libraries(ui PUBLIC Qt5::Widgets PUBLIC Qt5::Core)
target_include_directories(network PUBLIC "${CMAKE_SOURCE_DIR}/network"
  PUBLIC Qt5::Widgets PUBLIC Qt5::Core)
