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


# target_link_libraries(YourExecutable ${Boost_LIBRARIES})
# set_target_properties(pinger_test.elf PROPERTIES LINK_FLAGS "-lpthread")
