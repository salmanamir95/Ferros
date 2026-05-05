// PID ANALYZERS
// │
// ├── ProcessLifecycleAnalyzer
// ├── ProcessCPUUsageAnalyzer
// ├── ProcessBurstAnalyzer
// ├── ProcessSchedulingFrequencyAnalyzer
// ├── ProcessCoreAffinityAnalyzer
// ├── ProcessStabilityAnalyzer
// └── ProcessAnomalyAnalyzer

#pragma once

#include <unordered_map>
#include <limits>
#include <cstdint>
#include <array>

#include "analyzer/cpu/ICPUAnalyzer.h"
#include "telemetry/TelemetryBundle.h"
#include "events.h"
#include "analyzer/cpu/pid_analyzers/ProcessLifecycleInsight.h"

class ProcessLifecycleAnalyzer : public ICPUAnalyzer
{
public:
    void analyze(const TelemetryBundle& bundle) override;

    std::vector<ProcessLifecycleInsight> getInsights() const;

private:
    struct LifecycleState
    {
        u64 first_seen = std::numeric_limits<u64>::max();
        u64 last_seen  = 0;
        u64 event_count = 0;
        u32 exit_code   = 0;
        bool exited     = false;
        std::array<char, 16> comm{};
    };

    size_t last_processed = 0;
    

    std::unordered_map<u32, LifecycleState> state;

    void updateState(const cpu_event& e);
};