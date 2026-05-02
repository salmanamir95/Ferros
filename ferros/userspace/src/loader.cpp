#include "loader.h"

#include <iostream>
#include <csignal>
#include <unistd.h>

#include <bpf/libbpf.h>
#include <bpf/bpf.h>

#include "events.h"

static volatile bool running = true;

/* =========================================================
   Signal handler (Ctrl + C)
========================================================= */
static void handle_signal(int)
{
    running = false;
}

/* =========================================================
   Ring buffer callback
========================================================= */
static int handle_event(void *ctx, void *data, size_t size)
{
    cpu_event *e = (cpu_event *)data;

    std::cout
        << "PID: " << e->pid
        << " | RUNTIME(ns): " << e->runtime_ns
        << " | COMM: " << e->comm
        << std::endl;

    return 0;
}

/* =========================================================
   Main eBPF loader
========================================================= */
int start_ebpf()
{
    signal(SIGINT, handle_signal);

    libbpf_set_strict_mode(LIBBPF_STRICT_ALL);

    std::cout << "Opening eBPF object...\n";

    struct bpf_object *obj;

    obj = bpf_object__open_file(
        "../build/cpu_usage.bpf.o",
        NULL
    );

    if (!obj)
    {
        std::cerr << "Failed to open eBPF object\n";
        return 1;
    }

    std::cout << "Loading eBPF program...\n";

    if (bpf_object__load(obj))
    {
        std::cerr << "Failed to load eBPF program\n";
        bpf_object__close(obj);
        return 1;
    }

    /* =====================================================
       Find program
    ===================================================== */

    struct bpf_program *prog;

    prog = bpf_object__find_program_by_name(
        obj,
        "handle_sched_switch"
    );

    if (!prog)
    {
        std::cerr << "Failed to find program\n";
        bpf_object__close(obj);
        return 1;
    }

    /* =====================================================
       Attach tracepoint
    ===================================================== */

    struct bpf_link *link;

    link = bpf_program__attach_tracepoint(
        prog,
        "sched",
        "sched_switch"
    );

    if (!link)
    {
        std::cerr << "Failed to attach tracepoint\n";
        bpf_object__close(obj);
        return 1;
    }

    /* =====================================================
       Setup ring buffer
    ===================================================== */

    int map_fd = bpf_object__find_map_fd_by_name(
        obj,
        "events"
    );

    if (map_fd < 0)
    {
        std::cerr << "Failed to find ring buffer map\n";
        bpf_link__destroy(link);
        bpf_object__close(obj);
        return 1;
    }

    struct ring_buffer *rb;

    rb = ring_buffer__new(
        map_fd,
        handle_event,
        NULL,
        NULL
    );

    if (!rb)
    {
        std::cerr << "Failed to create ring buffer\n";
        bpf_link__destroy(link);
        bpf_object__close(obj);
        return 1;
    }

    /* =====================================================
       Running loop
    ===================================================== */

    std::cout << "eBPF attached successfully\n";
    std::cout << "Monitoring CPU scheduling...\n";
    std::cout << "Press Ctrl+C to stop\n";

    while (running)
    {
        ring_buffer__poll(rb, 100);
    }

    /* =====================================================
       Cleanup
    ===================================================== */

    ring_buffer__free(rb);
    bpf_link__destroy(link);
    bpf_object__close(obj);

    std::cout << "Exiting...\n";

    return 0;
}