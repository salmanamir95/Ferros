cmake_minimum_required(VERSION 3.16)

project(ferros C CXX)

# =========================================================
# Language Standards
# =========================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =========================================================
# Project Root
# =========================================================

set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})

# =========================================================
# Skeleton Policy (IMPORTANT)
# =========================================================
# ALL generated .skel.h files MUST live here
set(SKELETON_DIR ${PROJECT_ROOT}/userspace/inc)

# =========================================================
# Include Directories
# =========================================================

set(INCLUDE_DIRS
    ${PROJECT_ROOT}/shared/inc
    ${SKELETON_DIR}              # <-- skeletons ONLY here
    ${PROJECT_ROOT}/userspace/inc
)

# =========================================================
# Userspace Sources
# =========================================================

file(GLOB_RECURSE USERSPACE_SOURCES
    CONFIGURE_DEPENDS
    ${PROJECT_ROOT}/userspace/src/*.cpp
    ${PROJECT_ROOT}/userspace/src/*.c
    ${PROJECT_ROOT}/shared/src/*.cpp
    ${PROJECT_ROOT}/shared/src/*.c
)

# =========================================================
# Headers (IDE support)
# =========================================================

file(GLOB_RECURSE PROJECT_HEADERS
    CONFIGURE_DEPENDS
    ${PROJECT_ROOT}/userspace/inc/*.h
    ${PROJECT_ROOT}/shared/inc/*.h
)

# =========================================================
# eBPF Sources ONLY (.bpf.c)
# =========================================================

file(GLOB_RECURSE BPF_SOURCES
    CONFIGURE_DEPENDS
    ${PROJECT_ROOT}/ebpf/src/*.bpf.c
)

# =========================================================
# libbpf
# =========================================================

find_library(LIBBPF_LIB bpf REQUIRED)

# =========================================================
# Userspace executable
# =========================================================

add_executable(${PROJECT_NAME}
    ${USERSPACE_SOURCES}
    ${PROJECT_HEADERS}
    ${BPF_SKELS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${LIBBPF_LIB}
    elf
    z
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# =========================================================
# eBPF compilation
# =========================================================

set(BPF_OBJECTS "")

foreach(bpf_file ${BPF_SOURCES})

    get_filename_component(bpf_name ${bpf_file} NAME_WE)

    set(output_file
        ${CMAKE_BINARY_DIR}/${bpf_name}.bpf.o
    )
    set(skel_file
        ${SKELETON_DIR}/${bpf_name}.skel.h
    )

    add_custom_command(
        OUTPUT ${output_file}

        COMMAND clang
            -O2
            -g
            -target bpf
            -D__TARGET_ARCH_x86

            # eBPF includes ONLY kernel + shared headers
            -I${PROJECT_ROOT}/ebpf
            -I${PROJECT_ROOT}/shared/inc

            -c ${bpf_file}
            -o ${output_file}

        DEPENDS
            ${bpf_file}
            ${PROJECT_HEADERS}

        COMMENT "Building eBPF object: ${bpf_name}"
    )

    add_custom_command(
        OUTPUT ${skel_file}
        COMMAND bpftool gen skeleton ${output_file} > ${skel_file}
        DEPENDS ${output_file}
        COMMENT "Generating BPF skeleton: ${bpf_name}"
    )

    list(APPEND BPF_OBJECTS ${output_file})
    list(APPEND BPF_SKELS ${skel_file})

endforeach()

# =========================================================
# eBPF build target
# =========================================================

add_custom_target(ebpf_objects ALL
    DEPENDS ${BPF_OBJECTS}
)

# =========================================================
# IMPORTANT POLICY CHECK (skeleton enforcement)
# =========================================================

add_custom_target(check_skeleton_policy ALL
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "=== SKELETON POLICY ==="
    COMMAND ${CMAKE_COMMAND} -E echo "All *.skel.h MUST be in:"
    COMMAND ${CMAKE_COMMAND} -E echo "${SKELETON_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)