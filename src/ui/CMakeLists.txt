set(UI_MAIN_SRC pinger_main_window.cpp ping_charts.cpp)

qt5_wrap_cpp(UI_MOC_SRC pinger_main_window.hpp)
qt5_wrap_ui(UI_UIC_H designer/QtPingUI_1.ui)
add_library(ui OBJECT ${UI_MAIN_SRC} ${UI_MOC_SRC} ${UI_UIC_H} ping_charts.hpp)

target_link_libraries(ui PUBLIC network PUBLIC core
  PUBLIC Qt5::Charts Qt5::Widgets Qt5::Core Qt5::Gui)

target_include_directories(ui PUBLIC "${CMAKE_SOURCE_DIR}/network"
  PUBLIC "${CMAKE_SOURCE_DIR}/ui"
  PUBLIC "${CMAKE_SOURCE_DIR}/core"
  PUBLIC "${CMAKE_BINARY_DIR}/ui")
