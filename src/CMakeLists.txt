cmake_minimum_required(VERSION 3.5)
include(../cmake/common.cmake)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

add_library(dhash dhash.cpp)
target_link_libraries(dhash ${CONAN_PKG_LIBS_OPENSSL})
