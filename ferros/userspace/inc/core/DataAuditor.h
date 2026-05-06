#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "common/IInsight.h"
#include "analyzer/cpu/models/ModelWrappers.h"

/**
 * @brief Strict Observability Data Validator and Logic Auditor.
 * Enforces lossless, logically consistent execution traces.
 */
class DataAuditor {
public:
    struct Stats {
        u32 total_input = 0;
        u32 valid_output = 0;
        u32 removed = 0;
        u32 corrected = 0;
    };

    /**
     * @brief Audits, validates, and deduplicates the insight stream.
     */
    void audit(const std::vector<std::shared_ptr<IInsight>>& input, 
               std::vector<std::shared_ptr<IInsight>>& output);

private:
    Stats stats;
    std::vector<foc::InvalidRecord> logs;

    void validateSwitch(foc::ContextSwitchFrequencyAnalysis& a, bool& keep);
    void validateSlice(foc::ExecutionSliceAnalysis& a, bool& keep);
    void validateLifetime(foc::ProcessLifetimeAnalysis& a, bool& keep);
    
    // Add logic to merge fragmented records
    void mergeAndDeduplicate(std::vector<std::shared_ptr<IInsight>>& stream);
};
