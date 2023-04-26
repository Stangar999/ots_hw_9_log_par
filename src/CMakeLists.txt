cmake_minimum_required(VERSION 3.5)

project(log_par_p LANGUAGES CXX)

add_executable(log_par_e main.cpp
)
# TODO
# Чем отличается find_library от find_package
# и когда надо использовать find_library
# а когда достаточно и target_link_libraries

# Я думал что в target_link_libraries надо передавать конккретный путь,
# а если хочешь что бы CMake сам нашел библиотеку по имени
# надо find_library(имяпеременной куда сохраниться путь к библиотеке
# и название искомой библиотеки) типо find_library(CurrLib NAMES liblibasync_l)
# а потом эту переменную в target_link_libraries(log_par_e PRIVATE CurrLib)
# но так не работает

# Почему такая запись выдает ошибку
# find_library(CurrLib NAMES liblibasync_l)
# target_link_libraries(log_par_e PRIVATE CurrLib)

target_link_libraries(log_par_e PRIVATE libasync_l)
target_include_directories(log_par_e PRIVATE "../lib/libasync/")


