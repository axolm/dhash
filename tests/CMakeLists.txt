cmake_minimum_required(VERSION 3.5)
include(../cmake/common.cmake)

link_libraries(dhash)
link_libraries(${CONAN_PKG_LIBS_CATCH2})

add_executable(all_tests all_tests.cpp)
