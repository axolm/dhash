cmake_minimum_required(VERSION 3.5)
include(../cmake/common.cmake)

add_library(dhash dhash.cpp)
target_link_libraries(dhash ${OPENSSL_LIBRARIES})
