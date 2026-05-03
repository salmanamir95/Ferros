cmake_minimum_required(VERSION 3.16)
project(ferros C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})

set(SHARED_DIR    ${PROJECT_ROOT}/shared)
set(EBPF_DIR      ${PROJECT_ROOT}/ebpf)
set(USERSPACE_DIR ${PROJECT_ROOT}/userspace)

set(SKELETON_DIR ${CMAKE_BINARY_DIR}/skel)
file(MAKE_DIRECTORY ${SKELETON_DIR})

find_library(LIBBPF_LIB bpf REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(shared INTERFACE)

target_include_directories(shared INTERFACE
    ${SHARED_DIR}/inc
)

target_link_libraries(shared INTERFACE
    nlohmann_json::nlohmann_json
)

include(cmake/aebpf.cmake)
include(cmake/userspace.cmake)

add_executable(ferros ${USERSPACE_SOURCES})

target_include_directories(ferros PRIVATE
    ${USERSPACE_DIR}/inc
    ${SKELETON_DIR}
)

target_link_libraries(ferros
    shared
    ${LIBBPF_LIB}
    elf
    z
)

add_dependencies(ferros bpf_skel_target)