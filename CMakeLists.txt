cmake_minimum_required(VERSION 3.17)
project(silice-debugger)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(libs)

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/sourcePath.in"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/sourcePath.h"
)

INSTALL(TARGETS silice-debugger RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/../../bin)
