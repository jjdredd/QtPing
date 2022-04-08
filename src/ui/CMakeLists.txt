set(UI_MAIN_SRC pinger_main_window.cpp)

qt5_wrap_cpp(UI_MOC_SRC pinger_main_window.hpp QtPingerCore.hpp)
qt5_wrap_ui(UI_UIC_H designer/QtPingUI_1.ui)
add_library(ui OBJECT ${UI_MAIN_SRC} ${UI_MOC_SRC} ${UI_UIC_H})

target_link_libraries(ui PUBLIC Qt5::Widgets PUBLIC Qt5::Core)
target_include_directories(ui PUBLIC "${CMAKE_SOURCE_DIR}/network"
  PUBLIC "${CMAKE_SOURCE_DIR}/ui" PUBLIC "${CMAKE_BINARY_DIR}/ui")
