set(DHASH_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20) # TODO: support C++14

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set (COMPILER_CLANG 1)
endif()

if(CMAKE_GENERATOR STREQUAL "Ninja")
  if(COMPILER_CLANG)
    string(APPEND CMAKE_CXX_FLAGS " -fcolor-diagnostics")
  else()
    string(APPEND CMAKE_CXX_FLAGS " -fdiagnostics-color=always")
  endif()
endif()


add_compile_options("-g")
add_compile_options("-Wall" "-Wextra" "-Wpedantic" "-Werror")

if (COMPILER_CLANG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
