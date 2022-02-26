cmake_minimum_required(VERSION 3.18)

project(QtPing)

find_package(Boost 1.36.0 REQUIRED COMPONENTS program_options system)

# set CFLAGS etc
set(WFLAGS "-W -Wall -Wextra -g -std=c++17")
set(CMAKE_CXX_FLAGS  "${WFLAGS}")

include_directories(AFTER ${Boost_INCLUDE_DIR})
include_directories(AFTER "${CMake_SOURCE_DIR}/network")

add_subdirectory(network)
add_subdirectory(test)

# add_executable(main.elf main.cpp)
# set_target_properties(main.elf PROPERTIES LINK_FLAGS "-lpthread")
# target_link_libraries(main.elf PUBLIC network PUBLIC ${Boost_LIBRARIES})
