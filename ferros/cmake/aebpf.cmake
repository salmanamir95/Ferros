file(GLOB_RECURSE BPF_SOURCES
    ${PROJECT_ROOT}/ebpf/src/*.bpf.c
)

set(BPF_OBJECTS "")
set(BPF_SKELETONS "")

# ----------------------------
# Output directory (once)
# ----------------------------
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bpf)

# ----------------------------
# BPF include directories
# ----------------------------
set(BPF_INCLUDE_DIRS
    ${PROJECT_ROOT}/ebpf
    ${PROJECT_ROOT}/ebpf/inc
    ${SHARED_DIR}/inc
)

foreach(bpf ${BPF_SOURCES})

    get_filename_component(name ${bpf} NAME_WE)

    set(obj   ${CMAKE_BINARY_DIR}/bpf/${name}.bpf.o)
    set(skel  ${SKELETON_DIR}/${name}.skel.h)

    # ----------------------------
    # 1. Compile eBPF program
    # ----------------------------
    add_custom_command(
        OUTPUT ${obj}
        COMMAND clang
            -O2 -g
            -target bpf
            -D__TARGET_ARCH_x86
            -I${PROJECT_ROOT}/ebpf
            -I${PROJECT_ROOT}/ebpf/inc
            -I${SHARED_DIR}/inc
            -c ${bpf}
            -o ${obj}
        DEPENDS ${bpf}
        COMMENT "Compiling eBPF: ${name}"
    )

    # ----------------------------
    # 2. Generate skeleton
    # ----------------------------
    add_custom_command(
        OUTPUT ${skel}
        COMMAND bpftool gen skeleton ${obj} > ${skel}
        DEPENDS ${obj}
        COMMENT "Generating skeleton: ${name}"
    )

    list(APPEND BPF_OBJECTS ${obj})
    list(APPEND BPF_SKELETONS ${skel})

endforeach()

# ----------------------------
# Global build targets
# ----------------------------
add_custom_target(bpf_objects ALL DEPENDS ${BPF_OBJECTS})

add_custom_target(bpf_skel_target ALL DEPENDS ${BPF_SKELETONS})

add_dependencies(bpf_skel_target bpf_objects)