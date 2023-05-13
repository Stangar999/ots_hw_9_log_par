cmake_minimum_required(VERSION 3.22)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})

project(async VERSION ${PROJECT_VESRION})
configure_file(./lib/version_lib/version_define.h.in version_define.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(COMM_SANITIZER
#    "-fsanitize=undefined
#    -fsanitize-recover=all
#    -fsanitize=address
#    -fsanitize=leak
#    -fsanitize=memory
#    -fsanitize=thread" )
#set(CMAKE_CXX_FLAGS ${COMM_SANITIZER})
#set(CMAKE_C_FLAGS ${COMM_SANITIZER})
#set(CMAKE_LINKER_FLAGS ${COMM_SANITIZER})

add_subdirectory(./src)
add_subdirectory(./lib/version_lib)
add_subdirectory(./lib/libasync)

install(TARGETS libasync_l log_par_e RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
