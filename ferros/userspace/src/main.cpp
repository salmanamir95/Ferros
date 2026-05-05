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
    ProcessLifecycleAnalyzer * analyzer = new ProcessLifecycleAnalyzer();

    registry.registerAnalyzer(
        TelemetryType::CPU,
        analyzer
    );

    // ----------------------------
    // Start eBPF ingestion
    // ----------------------------
    int ret = start_ebpf(bundle, registry, analyzer);

    std::cerr << "Program exited with code: " << ret << std::endl;

    delete analyzer;
    return ret;
}