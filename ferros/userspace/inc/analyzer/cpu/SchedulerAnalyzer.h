#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <set>
#include "analyzer/cpu/ICPUAnalyzer.h"
#include "analyzer/cpu/SchedulerInsight.h"

class SchedulerAnalyzer : public ICPUAnalyzer {
public:
    void analyze(const TelemetryBundle& bundle) override;
    void collectInsights(std::vector<std::shared_ptr<IInsight>>& insights) override;

private:
    struct CpuState {
        u32 current_pid = 0;
        u64 last_ts = 0;
    };

    std::unordered_map<u32, CpuState> cpu_states;
    std::unordered_map<u32, ProcessInfo> process_map;
    std::unordered_map<u32, std::vector<u32>> tree;
    std::set<u32> roots;
    std::vector<std::string> anomalies;
    u32 total_processed = 0;

    void handleSwitch(const foc_event& ev);
    void handleFork(const foc_event& ev);
    void detectAnomalies(u32 pid, const ProcessInfo& info);
};
