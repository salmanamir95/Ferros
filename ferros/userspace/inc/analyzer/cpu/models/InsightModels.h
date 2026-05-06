#pragma once
#include <vector>
#include <string>
#include "types.h"

namespace foc {

// --- Inferred Insights (Layer 3) ---

struct CPUStarvationInsight {
    std::string insight_type = "cpu_starvation";
    u32 pid;
    std::string severity;
    struct { u64 expected_runtime_ns; u64 actual_runtime_ns; } evidence;
};

struct CPUHoggingInsight {
    std::string insight_type = "cpu_hog";
    u32 pid;
    double cpu_share_percent;
};

struct ForkBombInsight {
    std::string insight_type = "fork_bomb_risk";
    u32 root_pid;
    double fork_rate_per_sec;
    std::string growth_pattern;
};

struct SchedulerFairnessInsight {
    std::string insight_type = "scheduler_unfairness";
    std::vector<u32> affected_pids;
    double fairness_score;
};

struct PreemptionDominanceInsight {
    std::string insight_type = "preemption_dominance";
    u32 victim_pid;
    std::vector<u32> dominators;
    double preemption_ratio;
};

struct SystemPressureInsight {
    std::string insight_type = "system_pressure";
    std::string level;
    u32 switch_rate;
    double cpu_utilization_estimate;
};

struct BurstInsight {
    std::string insight_type = "burst_behavior";
    u32 pid;
    struct Burst { u64 active_ns; u64 idle_ns; };
    std::vector<Burst> burst_pattern;
};

// --- Pipeline Audit Models ---

struct InvalidRecord {
    std::string analysis_type = "invalid_record";
    std::string reason;
    std::string original_type;
};

struct InvalidRecordLog {
    std::string insight_type = "invalid_record_log";
    std::vector<InvalidRecord> invalid_records;
};

struct DataQualityReport {
    std::string insight_type = "data_quality_report";
    struct {
        u32 total_input;
        u32 valid_output;
        u32 removed;
        u32 corrected;
    } summary;
};

} // namespace foc
