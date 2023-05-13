cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(version_l version_define.h.in version.cpp)

target_include_directories(version_l
    PRIVATE "${CMAKE_BINARY_DIR}"
)
