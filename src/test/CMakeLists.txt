include_directories(AFTER ${Boost_INCLUDE_DIR})
include_directories(AFTER "${CMAKE_SOURCE_DIR}/network")

add_executable(pinger_test.elf pinger_test.cpp)

set_target_properties(pinger_test.elf PROPERTIES LINK_FLAGS "-lpthread")

target_link_libraries(pinger_test.elf PUBLIC network PUBLIC ${Boost_LIBRARIES})
