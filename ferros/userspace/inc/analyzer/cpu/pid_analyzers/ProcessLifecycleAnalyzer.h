#pragma once

#include <unordered_map>
#include <limits>
#include <cstdint>
#include <array>
#include <memory>
#include <vector>

#include "analyzer/cpu/ICPUAnalyzer.h"
#include "telemetry/TelemetryBundle.h"
#include "events.h"
#include "analyzer/cpu/pid_analyzers/ProcessLifecycleInsight.h"

class ProcessLifecycleAnalyzer : public ICPUAnalyzer
{
public:
    void analyze(const TelemetryBundle& bundle) override;
    
    void collectInsights(std::vector<std::shared_ptr<IInsight>>& insights) override;

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

    std::unordered_map<u32, LifecycleState> state;

    void updateFromSwitch(const foc_event& ev);
    void updateFromFork(const foc_event& ev);
};