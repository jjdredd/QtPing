include_directories(AFTER ${Boost_INCLUDE_DIR})
include_directories(AFTER "${CMAKE_SOURCE_DIR}/network")

add_executable(pinger_test.elf pinger_test.cpp)

target_link_libraries(pinger_test.elf PIBLIC network ${Boost_LIBRARIES})
