cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(libasync_l SHARED
command_handler.h
log_users.h
observer.h
async.h
async.cpp
)

target_link_libraries(libasync_l Threads::Threads)
