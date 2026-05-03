cmake_minimum_required(VERSION 3.16)
project(ferros C CXX)

# ----------------------------
# Language standards
# ----------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----------------------------
# Project structure
# ----------------------------
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})

set(SHARED_DIR    ${PROJECT_ROOT}/shared)
set(EBPF_DIR      ${PROJECT_ROOT}/ebpf)
set(USERSPACE_DIR ${PROJECT_ROOT}/userspace)

# ----------------------------
# Output directories
# ----------------------------
set(SKELETON_DIR ${USERSPACE_DIR}/bpf)
file(MAKE_DIRECTORY ${SKELETON_DIR})

# ----------------------------
# Dependencies
# ----------------------------
find_library(LIBBPF_LIB bpf REQUIRED)

# ============================================================
# SHARED (HEADER-ONLY CONTRACT LAYER)
# ============================================================
add_library(shared INTERFACE)

target_include_directories(shared INTERFACE
    ${SHARED_DIR}/inc
)

# ============================================================
# eBPF BUILD SYSTEM
# ============================================================
include(cmake/aebpf.cmake)

# ============================================================
# USERSPACE BUILD SYSTEM
# ============================================================
include(cmake/userspace.cmake)

# ============================================================
# FINAL EXECUTABLE
# ============================================================
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

# Ensure eBPF artifacts exist before linking userspace
add_dependencies(ferros bpf_skel_target)