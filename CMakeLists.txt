cmake_minimum_required(VERSION 3.5)
include(cmake/common.cmake)

project(dhash)


set(CONAN_INSTALL_DIR ${CMAKE_BINARY_DIR}/conan)
file(MAKE_DIRECTORY ${CONAN_INSTALL_DIR})
execute_process(
    COMMAND conan install "--build=missing" ${DHASH_ROOT}/conan/conanfile.txt
    # COMMAND conan install "--build=missing" --profile=${DHASH_ROOT}/conan/clang-libstdcpp.conf ${DHASH_ROOT}/conan/conanfile.txt
    COMMAND_ECHO STDOUT
    WORKING_DIRECTORY ${CONAN_INSTALL_DIR}
)
include(${CONAN_INSTALL_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
message("conan libs: ${CONAN_LIBS}")

# find_package(Boost REQUIRED)
# find_package(OpenSSL REQUIRED)
# find_package(Catch2 REQUIRED)

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
