cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

set ( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(libasync_l SHARED
command_handler.h
log_users.h
log_users.cpp
observer.h
async.h
async.cpp
)

target_link_libraries(libasync_l Threads::Threads)
