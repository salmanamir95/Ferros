#include "loader.h"

#include <iostream>
#include <iomanip>
#include <csignal>
#include <cstring>

#include <bpf/libbpf.h>

#include "cpu_usage.skel.h"
#include "events.h"

static volatile bool running = true;

static void handle_signal(int)
{
    running = false;
}

/* ring buffer callback */
static int handle_event(void *ctx, void *data, size_t size)
{
    if (size < sizeof(cpu_event))
        return 0;

    const cpu_event *e = (const cpu_event *)data;

    double time_sec = (double)e->timestamp_ns / 1e9;

    std::cout << std::left
              << "[" << std::fixed << std::setprecision(6)
              << time_sec << "] "
              << "CPU: " << std::setw(3) << e->cpu
              << " | PID: " << std::setw(7) << e->pid
              << " | COMM: " << std::setw(16) << e->comm
              << std::endl;

    return 0;
}

int start_ebpf()
{
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    libbpf_set_strict_mode(LIBBPF_STRICT_ALL);

    /* Load + verify */
    cpu_usage_bpf *skel = cpu_usage_bpf__open_and_load();
    if (!skel) {
        std::cerr << "Failed to load BPF skeleton\n";
        return 1;
    }

    /* Attach */
    if (cpu_usage_bpf__attach(skel)) {
        std::cerr << "Failed to attach BPF program\n";
        return 1;
    }

    std::cout << "BPF Attached Successfully\n";

    /* Ring buffer */
    int map_fd = bpf_map__fd(skel->maps.events);

    struct ring_buffer *rb = ring_buffer__new(map_fd, handle_event, NULL, NULL);
    if (!rb) {
        std::cerr << "Failed to create ring buffer\n";
        return 1;
    }

    std::cout << "TIMESTAMP        CPU   PID     COMM\n";
    std::cout << "----------------------------------------\n";

    /* Main loop */
    while (running)
    {
        int err = ring_buffer__poll(rb, 100);
        if (err == -EINTR)
            continue;
        if (err < 0) {
            std::cerr << "Ring buffer error: " << err << "\n";
            break;
        }
    }

    ring_buffer__free(rb);
    cpu_usage_bpf__destroy(skel);

    std::cout << "\nStopped.\n";
    return 0;
}