#include "core/loader.h"

#include <iostream>
#include <chrono>
#include <sys/resource.h>
#include <thread>
#include <atomic>
#include <cstring>

#include <bpf/libbpf.h>
#include <bpf/bpf.h>

#include "cpu_usage.skel.h"
#include "events.h"
#include "common/SPSCQueue.h"

// ----------------------------
// Ingestion Pipeline
// ----------------------------
static SPSCQueue<foc_event> g_event_queue(1024 * 32); 
static std::atomic<bool> g_running{true};

static int handle_event(void *ctx, void *data, size_t size)
{
    if (size < sizeof(foc_event))
        return 0;

    const foc_event *e = static_cast<const foc_event *>(data);

    if (!g_event_queue.push(*e)) {
        // Drop metric could be logged here
    }

    return 0;
}

// ----------------------------
// eBPF lifecycle
// ----------------------------
int start_ebpf(TelemetryBundle &bundle, Pipeline &pipeline)
{
    struct cpu_usage_bpf *skel = nullptr;
    struct ring_buffer *rb = nullptr;
    int err;
    
    rlimit rlim;
    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;

    if (setrlimit(RLIMIT_MEMLOCK, &rlim))
    {
        std::cerr << "Warning: RLIMIT_MEMLOCK not set\n";
    }

    skel = cpu_usage_bpf__open_and_load();
    if (!skel)
    {
        std::cerr << "Failed to load BPF skeleton\n";
        return 1;
    }

    err = cpu_usage_bpf__attach(skel);
    if (err)
    {
        std::cerr << "Failed to attach BPF\n";
        goto cleanup;
    }

    rb = ring_buffer__new(
        bpf_map__fd(skel->maps.events),
        handle_event,
        nullptr,
        nullptr
    );

    if (!rb)
    {
        std::cerr << "Failed to create ring buffer\n";
        goto cleanup;
    }

    std::cout << "Ferros Observability Core (FOC) started...\n";

    while (g_running)
    {
        err = ring_buffer__poll(rb, 1); 

        if (err < 0 && err != -EINTR) {
            std::cerr << "ring buffer error: " << err << "\n";
            break;
        }

        // Direct ingestion into the truth stream
        while (auto opt_ev = g_event_queue.pop()) {
            bundle.addEvent(*opt_ev);
        }

        pipeline.step(bundle);
        bundle.clear();
    }

cleanup:
    ring_buffer__free(rb);
    cpu_usage_bpf__destroy(skel);

    return err < 0 ? -err : 0;
}