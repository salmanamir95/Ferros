cmake_minimum_required(VERSION 3.16)
project(ferros C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})

# output directory for skeletons
set(SKELETON_DIR ${PROJECT_BINARY_DIR}/skel)
file(MAKE_DIRECTORY ${SKELETON_DIR})

find_library(LIBBPF_LIB bpf REQUIRED)

include(cmake/bpf.cmake)
include(cmake/userspace.cmake)

# final binary
add_executable(ferros ${USERSPACE_SOURCES})

target_include_directories(ferros PRIVATE
    userspace/inc
    shared/inc
    ${SKELETON_DIR}
)

target_link_libraries(ferros
    ${LIBBPF_LIB}
    elf
    z
)

add_dependencies(ferros bpf_skel_target)