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
# Include Directories (Userspace)
# =========================================================

set(INCLUDE_DIRS
    ${PROJECT_ROOT}/shared/inc
    ${PROJECT_ROOT}/userspace/inc
    ${PROJECT_ROOT}/ebpf/inc
    ${PROJECT_ROOT}/ebpf
)

include_directories(${INCLUDE_DIRS})

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
# Headers (for IDE support)
# =========================================================

file(GLOB_RECURSE PROJECT_HEADERS
    CONFIGURE_DEPENDS
    ${PROJECT_ROOT}/userspace/inc/*.h
    ${PROJECT_ROOT}/shared/inc/*.h
    ${PROJECT_ROOT}/ebpf/inc/*.h
)

# =========================================================
# eBPF Sources
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

    add_custom_command(

        OUTPUT ${output_file}

        COMMAND clang
            -O2
            -g
            -target bpf
            -D__TARGET_ARCH_x86

            # IMPORTANT: include ALL headers for BPF build
            -I${PROJECT_ROOT}/ebpf
            -I${PROJECT_ROOT}/ebpf/inc
            -I${PROJECT_ROOT}/shared/inc

            -c ${bpf_file}
            -o ${output_file}

        DEPENDS
            ${bpf_file}
            ${PROJECT_HEADERS}

        COMMENT "Building eBPF object: ${bpf_name}"
    )

    list(APPEND BPF_OBJECTS ${output_file})

endforeach()

# =========================================================
# eBPF build target
# =========================================================

add_custom_target(ebpf_objects ALL
    DEPENDS ${BPF_OBJECTS}
)

# =========================================================
# Debug output
# =========================================================

message(STATUS "")
message(STATUS "========== INCLUDE DIRS ==========")

foreach(dir ${INCLUDE_DIRS})
    message(STATUS ${dir})
endforeach()

message(STATUS "")
message(STATUS "========== USERSPACE SOURCES ==========")

foreach(file ${USERSPACE_SOURCES})
    message(STATUS ${file})
endforeach()

message(STATUS "")
message(STATUS "========== eBPF SOURCES ==========")

foreach(file ${BPF_SOURCES})
    message(STATUS ${file})
endforeach()

message(STATUS "")
message(STATUS "========== HEADERS ==========")

foreach(file ${PROJECT_HEADERS})
    message(STATUS ${file})
endforeach()

message(STATUS "")