cmake_minimum_required(VERSION 3.5)
include(../cmake/common.cmake)

add_executable(test test.cpp)
target_link_libraries(test twab)
