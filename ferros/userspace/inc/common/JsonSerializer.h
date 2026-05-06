#pragma once

#include <string>
#include <vector>
#include <memory>
#include "common/ISerializer.h"
#include "common/IInsight.h"
#include "analyzer/cpu/models/ModelWrappers.h"

class JsonSerializer : public ISerializer {
public:
    JsonSerializer() = default;
    virtual ~JsonSerializer() override = default;
    
    // ISerializer interface
    std::string serialize(const IInsight& insight) override;

private:
    // Analysis Overloads
    static std::string serializeAnalysis(const foc::ExecutionSliceAnalysis& a);
    static std::string serializeAnalysis(const foc::ContextSwitchFrequencyAnalysis& a);
    static std::string serializeAnalysis(const foc::ProcessLifetimeAnalysis& a);
    static std::string serializeAnalysis(const foc::CPUAffinityAnalysis& a);
    static std::string serializeAnalysis(const foc::ProcessTreeAnalysis& a);
    static std::string serializeAnalysis(const foc::CPUMigrationAnalysis& a);

    // Insight Overloads
    static std::string serializeInsight(const foc::CPUStarvationInsight& i);
    static std::string serializeInsight(const foc::CPUHoggingInsight& i);
    static std::string serializeInsight(const foc::ForkBombInsight& i);
    static std::string serializeInsight(const foc::SchedulerFairnessInsight& i);
    static std::string serializeInsight(const foc::PreemptionDominanceInsight& i);
    static std::string serializeInsight(const foc::SystemPressureInsight& i);
    static std::string serializeInsight(const foc::BurstInsight& i);
};
