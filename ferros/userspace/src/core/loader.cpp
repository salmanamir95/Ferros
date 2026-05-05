#include "core/loader.h"

#include <iostream>
#include <chrono>
#include <sys/resource.h>

#include <bpf/libbpf.h>
#include <bpf/bpf.h>

#include "cpu_usage.skel.h"
#include "events.h"
#include "common/Serialize.h"
#include "analyzer/cpu/pid_analyzers/ProcessLifecycleAnalyzer.h"
// ----------------------------
// eBPF callback
// ----------------------------
static int handle_event(void *ctx, void *data, size_t size, ProcessLifecycleAnalyzer *analyzer)
{
    auto *bundle = static_cast<TelemetryBundle *>(ctx);

    const cpu_event *e = static_cast<const cpu_event *>(data);

    if (size < sizeof(cpu_event))
        return 0;

    // ----------------------------
    // Route into CPU telemetry
    // ----------------------------
    bundle->cpu().addEvent(*e);

    return 0;
}

// ----------------------------
// eBPF lifecycle
// ----------------------------
int start_ebpf(TelemetryBundle &bundle, AnalyzerRegistry &registry)
{
    struct cpu_usage_bpf *skel = nullptr;
    struct ring_buffer *rb = nullptr;
    int err;

    auto end = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    // ----------------------------
    // Allow BPF memory locking
    // ----------------------------
    rlimit rlim;
    rlim.rlim_cur = RLIM_INFINITY;
    rlim.rlim_max = RLIM_INFINITY;

    if (setrlimit(RLIMIT_MEMLOCK, &rlim))
    {
        std::cerr << "Warning: RLIMIT_MEMLOCK not set\n";
    }

    // ----------------------------
    // Load BPF program
    // ----------------------------
    skel = cpu_usage_bpf__open_and_load();
    if (!skel)
    {
        std::cerr << "Failed to load BPF skeleton\n";
        return 1;
    }

    // ----------------------------
    // Attach BPF hooks
    // ----------------------------
    err = cpu_usage_bpf__attach(skel);
    if (err)
    {
        std::cerr << "Failed to attach BPF\n";
        goto cleanup;
    }

    // ----------------------------
    // Ring buffer setup
    // ----------------------------
    rb = ring_buffer__new(
        bpf_map__fd(skel->maps.events),
        handle_event,
        &bundle,
        nullptr
    );

    if (!rb)
    {
        std::cerr << "Failed to create ring buffer\n";
        goto cleanup;
    }

    // ----------------------------
    // Runtime loop (1s window)
    // ----------------------------
    

    while (true)
    {
        err = ring_buffer__poll(rb, 10);

        if (err == -EINTR)
            continue;

        if (err < 0)
        {
            std::cerr << "ring buffer error: " << err << "\n";
            break;
        }
         registry.runAll(bundle);
        
        auto insights = analyzer.getInsights();

        std::string json =
        serialize::toPrettyString(insights);

        std::cout << json << std::endl;
        
        if (std::chrono::steady_clock::now() >= end)
            break;

    }

cleanup:
    ring_buffer__free(rb);
    cpu_usage_bpf__destroy(skel);

    return err < 0 ? -err : 0;
}