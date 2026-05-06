#include <iostream>
#include <memory>

#include "core/loader.h"
#include "telemetry/TelemetryBundle.h"
#include "core/Pipeline.h"
#include "analyzer/cpu/pid_analyzers/ProcessLifecycleAnalyzer.h"
#include "analyzer/cpu/SchedulerAnalyzer.h"
#include "common/JsonSerializer.h"
#include "storage/FileStorage.h"
#include "publisher/FrontendPublisher.h"

int main()
{
    // 1. Central telemetry container
    TelemetryBundle bundle;

    // 2. Initialize Pipeline
    Pipeline pipeline;

    // 3. Add Analyzers
    pipeline.addAnalyzer(std::make_unique<ProcessLifecycleAnalyzer>());
    pipeline.addAnalyzer(std::make_unique<SchedulerAnalyzer>());

    // 4. Set Serializer
    pipeline.setSerializer(std::make_unique<JsonSerializer>());

    // 5. Add Publishers
    pipeline.addPublisher(std::make_unique<FileStorage>("JSON"));
    pipeline.addPublisher(std::make_unique<FrontendPublisher>("JSON"));

    // 6. Start eBPF ingestion
    std::cout << "Starting Ferros Pipeline..." << std::endl;
    int ret = start_ebpf(bundle, pipeline);

    std::cerr << "Program exited with code: " << ret << std::endl;
    return ret;
}