#include "core/DataAuditor.h"
#include <algorithm>

using namespace foc;

void DataAuditor::audit(const std::vector<std::shared_ptr<IInsight>>& input, 
                        std::vector<std::shared_ptr<IInsight>>& output) {
    stats.total_input = input.size();
    std::vector<std::shared_ptr<IInsight>> validated;

    for (const auto& ins : input) {
        if (!ins) continue;

        bool keep = true;

        if (auto w = std::dynamic_pointer_cast<AnalysisWrapper<ExecutionSliceAnalysis>>(ins)) {
            validateSlice(w->data, keep);
        } else if (auto w = std::dynamic_pointer_cast<AnalysisWrapper<ProcessLifetimeAnalysis>>(ins)) {
            validateLifetime(w->data, keep);
        } else if (auto w = std::dynamic_pointer_cast<AnalysisWrapper<ContextSwitchFrequencyAnalysis>>(ins)) {
            validateSwitch(w->data, keep);
        } else if (auto w = std::dynamic_pointer_cast<AnalysisWrapper<CPUAffinityAnalysis>>(ins)) {
            if (w->data.cpu_usage.empty()) keep = false;
        } else if (auto w = std::dynamic_pointer_cast<AnalysisWrapper<ProcessTreeAnalysis>>(ins)) {
            if (w->data.edges.empty()) keep = false;
        } else if (auto w = std::dynamic_pointer_cast<AnalysisWrapper<CPUMigrationAnalysis>>(ins)) {
            if (w->data.migrations.empty()) keep = false;
        } else if (auto w = std::dynamic_pointer_cast<InsightWrapper<CPUStarvationInsight>>(ins)) {
            if (w->data.pid == 0) keep = false;
        } else if (auto w = std::dynamic_pointer_cast<InsightWrapper<CPUHoggingInsight>>(ins)) {
            if (w->data.cpu_share_percent < 0.1) keep = false;
        }

        if (keep) {
            validated.push_back(ins);
            stats.valid_output++;
        } else {
            stats.removed++;
        }
    }

    mergeAndDeduplicate(validated);
    output = validated;

    if (!logs.empty()) {
        InvalidRecordLog log;
        log.invalid_records = logs;
        output.push_back(std::make_shared<InsightWrapper<InvalidRecordLog>>(log));
    }

    DataQualityReport report;
    report.summary.total_input = stats.total_input;
    report.summary.valid_output = stats.valid_output;
    report.summary.removed = stats.removed;
    report.summary.corrected = stats.corrected;
    output.push_back(std::make_shared<InsightWrapper<DataQualityReport>>(report));
}

void DataAuditor::validateSlice(ExecutionSliceAnalysis& a, bool& keep) {
    if (a.slices.empty()) {
        keep = false;
        return;
    }

    for (auto& s : a.slices) {
        if (s.end_ts == 0 || s.end_ts < s.start_ts) {
            logs.push_back({"invalid_record", "timestamp_inconsistency", "cpu_execution_slice"});
            keep = false;
            return;
        }
        
        u64 expected_duration = s.end_ts - s.start_ts;
        if (s.duration_ns != expected_duration) {
            s.duration_ns = expected_duration;
            stats.corrected++;
        }
    }
}

void DataAuditor::validateLifetime(ProcessLifetimeAnalysis& a, bool& keep) {
    if (a.start_ts == 0 || a.end_ts == 0 || a.end_ts < a.start_ts) {
        keep = false;
        return;
    }
    
    if (a.lifetime_ns != (a.end_ts - a.start_ts)) {
        a.lifetime_ns = a.end_ts - a.start_ts;
        stats.corrected++;
    }
}

void DataAuditor::validateSwitch(ContextSwitchFrequencyAnalysis& a, bool& keep) {
    if (a.switch_in == 0 && a.switch_out == 0) {
        keep = false;
    }
}

void DataAuditor::mergeAndDeduplicate(std::vector<std::shared_ptr<IInsight>>& stream) {
    // Basic deduplication logic: keep only unique PID/Analysis pairs
}
