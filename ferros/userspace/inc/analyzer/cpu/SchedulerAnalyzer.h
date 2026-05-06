#pragma once

#include <unordered_map>
#include <memory>
#include <vector>
#include <set>
#include "analyzer/cpu/ICPUAnalyzer.h"
#include "analyzer/cpu/models/ModelWrappers.h"

class SchedulerAnalyzer : public ICPUAnalyzer {
public:
    void analyze(const TelemetryBundle& bundle) override;
    void collectInsights(std::vector<std::shared_ptr<IInsight>>& insights) override;

private:
    // Internal State for Deterministic Reconstruction
    struct ProcessState {
        u32 pid;
        std::string comm;
        u64 total_runtime_ns = 0;
        u64 start_ts = 0;
        u64 last_ts = 0;
        u32 switch_in = 0;
        u32 switch_out = 0;
        std::unordered_map<u32, u64> cpu_runtime; // cpu -> ns
        std::vector<foc::Slice> slices;
        std::vector<foc::MigrationRecord> migrations;
        u32 last_cpu = 0xFFFFFFFF;

        // For Behavioral Inference
        u32 preemption_count = 0;
        std::vector<u32> dominators; // PIDs that preempted this one
    };

    struct CpuState {
        u32 current_pid = 0;
        u64 last_ts = 0;
        u32 total_switches = 0;
    };

    std::unordered_map<u32, CpuState> cpu_states;
    std::unordered_map<u32, ProcessState> process_states;
    std::vector<std::pair<u32, u32>> tree_edges;
    u64 total_runtime_global = 0;
    u64 last_global_ts = 0;

    void handleSwitch(const foc_event& ev);
    void handleFork(const foc_event& ev);
    
    // Tier 2: Deterministic Generation
    void generateAnalyses(std::vector<std::shared_ptr<IInsight>>& insights);
    
    // Tier 3: Behavioral Inference
    void generateInsights(std::vector<std::shared_ptr<IInsight>>& insights);
};
