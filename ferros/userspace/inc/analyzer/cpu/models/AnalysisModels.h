#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "types.h"

namespace foc {

struct Slice {
    u64 start_ts;
    u64 end_ts;
    u64 duration_ns;
};

struct ExecutionSliceAnalysis {
    std::string analysis_type = "cpu_execution_slice";
    u32 pid;
    u32 cpu;
    std::vector<Slice> slices;
    u64 total_runtime_ns;
};

struct ContextSwitchFrequencyAnalysis {
    std::string analysis_type = "context_switch_frequency";
    u32 pid;
    u32 switch_in;
    u32 switch_out;
    double switch_rate_per_sec;
};

struct ProcessLifetimeAnalysis {
    std::string analysis_type = "process_lifetime";
    u32 pid;
    u64 start_ts;
    u64 end_ts;
    u64 lifetime_ns;
};

struct CPUAffinityAnalysis {
    std::string analysis_type = "cpu_affinity_distribution";
    u32 pid;
    std::unordered_map<u32, double> cpu_usage; // cpu_id -> usage_percent
};

struct ProcessTreeAnalysis {
    std::string analysis_type = "process_tree";
    u32 root_pid;
    struct Edge { u32 parent; u32 child; };
    std::vector<Edge> edges;
};

struct MigrationRecord { u32 from_cpu; u32 to_cpu; u64 ts; };
struct CPUMigrationAnalysis {
    std::string analysis_type = "cpu_migration";
    u32 pid;
    std::vector<MigrationRecord> migrations;
};

} // namespace foc
