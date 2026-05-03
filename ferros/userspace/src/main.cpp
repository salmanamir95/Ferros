#include <iostream>

#include "core/loader.h"
#include "telemetry/TelemetryBundle.h"
#include "analyzer/AnalyzerRegistry.h"
#include "analyzer/cpu/pid_analyzers/ProcessLifecycleAnalyzer.h"
int main()
{
    // ----------------------------
    // Central telemetry container
    // ----------------------------
    TelemetryBundle bundle;
    AnalyzerRegistry registry;

    registry.registerAnalyzer(
        TelemetryType::CPU,
        new ProcessLifecycleAnalyzer()
    );

    // ----------------------------
    // Start eBPF ingestion
    // ----------------------------
    int ret = start_ebpf(bundle, registry);

    std::cerr << "Program exited with code: " << ret << std::endl;

    return ret;
}