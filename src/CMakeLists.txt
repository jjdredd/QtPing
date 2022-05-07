cmake_minimum_required(VERSION 3.18)

project(QtPing VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


find_package(Boost 1.36.0 REQUIRED COMPONENTS program_options system REQUIRED)
find_package(Qt5 COMPONENTS Widgets Core Gui Charts REQUIRED)


# set CFLAGS etc
set(WFLAGS "-W -Wall -Wextra -g -std=c++20")
set(CMAKE_CXX_FLAGS  "${WFLAGS}")

include_directories(AFTER ${Boost_INCLUDE_DIR})
include_directories(AFTER "${CMake_SOURCE_DIR}/network")
include_directories(AFTER "${CMake_SOURCE_DIR}/core")
include_directories(AFTER "${CMake_SOURCE_DIR}/ui")

add_subdirectory(network)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(test)

add_executable(main.elf main.cpp)
set_target_properties(main.elf PROPERTIES LINK_FLAGS "-lpthread")
target_link_libraries(main.elf
  PUBLIC network
  PUBLIC core
  PUBLIC ui
  PUBLIC ${Boost_LIBRARIES}
  PUBLIC Qt5::Widgets PUBLIC Qt5::Core)

