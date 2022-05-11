cmake_minimum_required(VERSION 3.5)
include(cmake/common.cmake)

project(twab)


set(CONAN_INSTALL_DIR ${CMAKE_BINARY_DIR}/conan)
file(MAKE_DIRECTORY ${CONAN_INSTALL_DIR})
execute_process(
    COMMAND conan install "--build=missing" ${TWAB_ROOT}/conan/conanfile.txt
    # COMMAND conan install "--build=missing" --profile=${TWAB_ROOT}/conan/clang-libstdcpp.conf ${TWAB_ROOT}/conan/conanfile.txt
    COMMAND_ECHO STDOUT
    WORKING_DIRECTORY ${CONAN_INSTALL_DIR}
)
include(${CONAN_INSTALL_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
message("conan libs: ${CONAN_LIBS}")

find_package(OpenSSL REQUIRED)

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
