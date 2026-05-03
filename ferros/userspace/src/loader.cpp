#include "loader.h"

#include <iostream>
#include <iomanip>
#include <csignal>
#include <cstring>
#include <sys/resource.h>

#include <bpf/libbpf.h>
#include <bpf/bpf.h>

#include "cpu_usage.skel.h"
#include "events.h"

static volatile bool running = true;

static void handle_signal(int)
{
    running = false;
}

static int handle_event(void * /*ctx*/, void *data, size_t size)
{
    const struct cpu_event *e = (const struct cpu_event *)data;

    if (size < sizeof(struct cpu_event))
        return 0;

    // Convert nanoseconds to seconds for display
    double time_sec = (double)e->timestamp_ns / 1e9;

    std::cout << std::left
              << std::setw(15) << std::fixed << std::setprecision(6) << time_sec
              << " CPU[" << std::setw(2) << e->cpu << "] "
              << " PID: " << std::setw(7) << e->pid
              << " COMM: " << std::setw(16) << e->comm
              << std::endl;

    return 0;
}

int start_ebpf()
{
    struct cpu_usage_bpf *skel;
    struct ring_buffer *rb = NULL;
    int err;

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    /* Increase RLIMIT_MEMLOCK for BPF maps */
    struct rlimit rlim;
    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_MEMLOCK, &rlim)) {
        std::cerr << "Warning: Failed to increase RLIMIT_MEMLOCK\n";
    }

    /* Open and load BPF application */
    skel = cpu_usage_bpf__open_and_load();
    if (!skel) {
        std::cerr << "Failed to open and load BPF skeleton\n";
        return 1;
    }

    /* Attach tracepoints */
    err = cpu_usage_bpf__attach(skel);
    if (err) {
        std::cerr << "Failed to attach BPF skeleton\n";
        goto cleanup;
    }

    /* Set up ring buffer polling */
    rb = ring_buffer__new(bpf_map__fd(skel->maps.events), handle_event, NULL, NULL);
    if (!rb) {
        std::cerr << "Failed to create ring buffer\n";
        goto cleanup;
    }

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " FERROS eBPF MONITOR STARTED\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << std::left << std::setw(15) << "TIMESTAMP" 
              << " DETAILS\n";
    std::cout << std::string(60, '-') << std::endl;

    while (running) {
        err = ring_buffer__poll(rb, 100);
        if (err == -EINTR) {
            err = 0;
            continue;
        }
        if (err < 0) {
            std::cerr << "Error polling ring buffer: " << err << "\n";
            break;
        }
    }

cleanup:
    ring_buffer__free(rb);
    cpu_usage_bpf__destroy(skel);
    return err < 0 ? -err : 0;
}