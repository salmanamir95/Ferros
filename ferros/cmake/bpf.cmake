file(GLOB_RECURSE BPF_SOURCES
    ${PROJECT_ROOT}/bpf/src/*.bpf.c
)

set(BPF_OBJECTS "")
set(BPF_SKELETONS "")

foreach(bpf ${BPF_SOURCES})

    get_filename_component(name ${bpf} NAME_WE)

    set(obj ${CMAKE_BINARY_DIR}/${name}.bpf.o)
    set(skel ${SKELETON_DIR}/${name}.skel.h)

    # 1. compile eBPF program
    add_custom_command(
        OUTPUT ${obj}
        COMMAND clang
            -O2 -g
            -target bpf
            -D__TARGET_ARCH_x86
            -I${PROJECT_ROOT}/bpf/inc
            -I${PROJECT_ROOT}/shared/inc
            -c ${bpf}
            -o ${obj}
        DEPENDS ${bpf}
        COMMENT "Compiling BPF: ${name}"
    )

    # 2. generate skeleton
    add_custom_command(
        OUTPUT ${skel}
        COMMAND bpftool gen skeleton ${obj} > ${skel}
        DEPENDS ${obj}
        COMMENT "Generating skeleton: ${name}"
    )

    list(APPEND BPF_OBJECTS ${obj})
    list(APPEND BPF_SKELETONS ${skel})

endforeach()

# single build target for all BPF artifacts
add_custom_target(bpf_objects ALL DEPENDS ${BPF_OBJECTS})

add_custom_target(bpf_skel_target ALL DEPENDS ${BPF_SKELETONS})

add_dependencies(bpf_skel_target bpf_objects)