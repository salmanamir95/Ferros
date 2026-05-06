#include "common/JsonSerializer.h"
#include <sstream>
#include <iomanip>

using namespace foc;

std::string JsonSerializer::serialize(const IInsight& ins_ref) {
    if (auto w = dynamic_cast<const AnalysisWrapper<ExecutionSliceAnalysis>*>(&ins_ref)) return serializeAnalysis(w->data);
    if (auto w = dynamic_cast<const AnalysisWrapper<ContextSwitchFrequencyAnalysis>*>(&ins_ref)) return serializeAnalysis(w->data);
    if (auto w = dynamic_cast<const AnalysisWrapper<ProcessLifetimeAnalysis>*>(&ins_ref)) return serializeAnalysis(w->data);
    if (auto w = dynamic_cast<const AnalysisWrapper<CPUAffinityAnalysis>*>(&ins_ref)) return serializeAnalysis(w->data);
    if (auto w = dynamic_cast<const AnalysisWrapper<ProcessTreeAnalysis>*>(&ins_ref)) return serializeAnalysis(w->data);
    if (auto w = dynamic_cast<const AnalysisWrapper<CPUMigrationAnalysis>*>(&ins_ref)) return serializeAnalysis(w->data);
    
    if (auto w = dynamic_cast<const InsightWrapper<CPUStarvationInsight>*>(&ins_ref)) return serializeInsight(w->data);
    if (auto w = dynamic_cast<const InsightWrapper<CPUHoggingInsight>*>(&ins_ref)) return serializeInsight(w->data);
    if (auto w = dynamic_cast<const InsightWrapper<PreemptionDominanceInsight>*>(&ins_ref)) return serializeInsight(w->data);
    if (auto w = dynamic_cast<const InsightWrapper<SystemPressureInsight>*>(&ins_ref)) return serializeInsight(w->data);
    if (auto w = dynamic_cast<const InsightWrapper<ForkBombInsight>*>(&ins_ref)) return serializeInsight(w->data);
    
    if (auto w = dynamic_cast<const InsightWrapper<InvalidRecordLog>*>(&ins_ref)) {
        std::stringstream ss;
        ss << "  {\n    \"invalid_records\": [";
        for (size_t i = 0; i < w->data.invalid_records.size(); ++i) {
            ss << "\n      {\"reason\": \"" << w->data.invalid_records[i].reason 
               << "\", \"original_type\": \"" << w->data.invalid_records[i].original_type << "\"}";
            if (i < w->data.invalid_records.size() - 1) ss << ",";
        }
        ss << "\n    ]\n  }";
        return ss.str();
    }

    if (auto w = dynamic_cast<const InsightWrapper<DataQualityReport>*>(&ins_ref)) {
        std::stringstream ss;
        ss << "  {\n    \"summary\": {\n      \"total_input\": " << w->data.summary.total_input 
           << ",\n      \"valid_output\": " << w->data.summary.valid_output 
           << ",\n      \"removed\": " << w->data.summary.removed 
           << ",\n      \"corrected\": " << w->data.summary.corrected << "\n    }\n  }";
        return ss.str();
    }
    
    return "  {}";
}

std::string JsonSerializer::serializeAnalysis(const ExecutionSliceAnalysis& a) {
    std::stringstream ss;
    ss << "  {\n    \"analysis_type\": \"cpu_execution_slice\",\n    \"pid\": " << a.pid 
       << ",\n    \"cpu\": " << a.cpu << ",\n    \"slices\": [";
    for (size_t i = 0; i < a.slices.size(); ++i) {
        ss << "\n      {\"start_ts\": " << a.slices[i].start_ts << ", \"end_ts\": " << a.slices[i].end_ts 
           << ", \"duration_ns\": " << a.slices[i].duration_ns << "}";
        if (i < a.slices.size() - 1) ss << ",";
    }
    ss << "\n    ],\n    \"total_runtime_ns\": " << a.total_runtime_ns << "\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeAnalysis(const ContextSwitchFrequencyAnalysis& a) {
    std::stringstream ss;
    ss << "  {\n    \"analysis_type\": \"context_switch_frequency\",\n    \"pid\": " << a.pid 
       << ",\n    \"switch_in\": " << a.switch_in << ",\n    \"switch_out\": " << a.switch_out 
       << ",\n    \"switch_rate_per_sec\": " << std::fixed << std::setprecision(2) << a.switch_rate_per_sec << "\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeAnalysis(const ProcessLifetimeAnalysis& a) {
    std::stringstream ss;
    ss << "  {\n    \"analysis_type\": \"process_lifetime\",\n    \"pid\": " << a.pid 
       << ",\n    \"start_ts\": " << a.start_ts << ",\n    \"end_ts\": " << a.end_ts 
       << ",\n    \"lifetime_ns\": " << a.lifetime_ns << "\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeAnalysis(const CPUAffinityAnalysis& a) {
    std::stringstream ss;
    ss << "  {\n    \"analysis_type\": \"cpu_affinity_distribution\",\n    \"pid\": " << a.pid 
       << ",\n    \"cpu_usage\": {";
    size_t count = 0;
    for (auto& [cpu, usage] : a.cpu_usage) {
        ss << "\n      \"" << cpu << "\": " << usage;
        if (++count < a.cpu_usage.size()) ss << ",";
    }
    ss << "\n    }\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeAnalysis(const ProcessTreeAnalysis& a) {
    std::stringstream ss;
    ss << "  {\n    \"analysis_type\": \"process_tree\",\n    \"root_pid\": " << a.root_pid 
       << ",\n    \"edges\": [";
    for (size_t i = 0; i < a.edges.size(); ++i) {
        ss << "\n      {\"parent\": " << a.edges[i].parent << ", \"child\": " << a.edges[i].child << "}";
        if (i < a.edges.size() - 1) ss << ",";
    }
    ss << "\n    ]\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeAnalysis(const CPUMigrationAnalysis& a) {
    std::stringstream ss;
    ss << "  {\n    \"analysis_type\": \"cpu_migration\",\n    \"pid\": " << a.pid 
       << ",\n    \"migrations\": [";
    for (size_t i = 0; i < a.migrations.size(); ++i) {
        ss << "\n      {\"from_cpu\": " << a.migrations[i].from_cpu << ", \"to_cpu\": " << a.migrations[i].to_cpu 
           << ", \"ts\": " << a.migrations[i].ts << "}";
        if (i < a.migrations.size() - 1) ss << ",";
    }
    ss << "\n    ]\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeInsight(const CPUStarvationInsight& i) {
    std::stringstream ss;
    ss << "  {\n    \"insight_type\": \"cpu_starvation\",\n    \"pid\": " << i.pid 
       << ",\n    \"severity\": \"" << i.severity << "\",\n    \"evidence\": {"
       << "\n      \"expected_runtime_ns\": " << i.evidence.expected_runtime_ns 
       << ",\n      \"actual_runtime_ns\": " << i.evidence.actual_runtime_ns << "\n    }\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeInsight(const CPUHoggingInsight& i) {
    std::stringstream ss;
    ss << "  {\n    \"insight_type\": \"cpu_hog\",\n    \"pid\": " << i.pid 
       << ",\n    \"cpu_share_percent\": " << std::fixed << std::setprecision(2) << i.cpu_share_percent << "\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeInsight(const ForkBombInsight& i) {
    std::stringstream ss;
    ss << "  {\n    \"insight_type\": \"fork_bomb_risk\",\n    \"root_pid\": " << i.root_pid 
       << ",\n    \"fork_rate_per_sec\": " << std::fixed << std::setprecision(2) << i.fork_rate_per_sec 
       << ",\n    \"growth_pattern\": \"" << i.growth_pattern << "\"\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeInsight(const PreemptionDominanceInsight& i) {
    std::stringstream ss;
    ss << "  {\n    \"insight_type\": \"preemption_dominance\",\n    \"victim_pid\": " << i.victim_pid 
       << ",\n    \"dominators\": [";
    for (size_t idx = 0; idx < i.dominators.size(); ++idx) {
        ss << i.dominators[idx];
        if (idx < i.dominators.size() - 1) ss << ", ";
    }
    ss << "],\n    \"preemption_ratio\": " << std::fixed << std::setprecision(2) << i.preemption_ratio << "\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeInsight(const SystemPressureInsight& i) {
    std::stringstream ss;
    ss << "  {\n    \"insight_type\": \"system_pressure\",\n    \"level\": \"" << i.level 
       << "\",\n    \"switch_rate\": " << i.switch_rate 
       << ",\n    \"cpu_utilization_estimate\": " << std::fixed << std::setprecision(2) << i.cpu_utilization_estimate << "\n  }";
    return ss.str();
}

std::string JsonSerializer::serializeInsight(const BurstInsight& i) {
    std::stringstream ss;
    ss << "  {\n    \"insight_type\": \"burst_behavior\",\n    \"pid\": " << i.pid 
       << ",\n    \"burst_pattern\": [";
    for (size_t idx = 0; idx < i.burst_pattern.size(); ++idx) {
        ss << "\n      {\"active_ns\": " << i.burst_pattern[idx].active_ns << ", \"idle_ns\": " << i.burst_pattern[idx].idle_ns << "}";
        if (idx < i.burst_pattern.size() - 1) ss << ",";
    }
    ss << "\n    ]\n  }";
    return ss.str();
}
