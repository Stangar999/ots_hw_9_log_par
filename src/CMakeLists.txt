cmake_minimum_required(VERSION 3.5)

project(log_par_p LANGUAGES CXX)

add_executable(log_par_e main.cpp
)

#find_library(LibAsync NAMES libasync_l HINTS "${CMAKE_BINARY_DIR}/lib/libasync")
#target_link_libraries(log_par_e PRIVATE "${LibAsync}")

#target_include_directories(log_par_e PRIVATE "../lib/libasync")

# или просто
target_link_libraries(log_par_e PRIVATE libasync_l)
target_include_directories(log_par_e PRIVATE "../lib/libasync")
# так как libasync_l создается в этом проекте, а target_link_libraries гибче,
#см. https://cmake.org/cmake/help/latest/command/target_link_libraries.html#id3
#(в частности, если в вашем проекте или подпроекте создаете библиотеку somename,
#то можете передавать в target_link_libraries это имя цели, somename.
#Cmake под капотом сам заменит его на полный путь.



