#include <iostream>

#include "core/loader.h"
#include "telemetry/TelemetryBundle.h"

int main()
{
    // ----------------------------
    // Central telemetry container
    // ----------------------------
    TelemetryBundle bundle;

    // ----------------------------
    // Start eBPF ingestion
    // ----------------------------
    int ret = start_ebpf(bundle);

    std::cerr << "Program exited with code: " << ret << std::endl;

    return ret;
}