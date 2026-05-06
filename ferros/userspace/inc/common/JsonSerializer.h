#pragma once

#include "common/ISerializer.h"
#include "common/Serialize.h"
#include "analyzer/cpu/pid_analyzers/ProcessLifecycleInsight.h"
#include "analyzer/cpu/SchedulerInsight.h"

/**
 * @brief JSON implementation of the ISerializer interface.
 */
class JsonSerializer : public ISerializer {
public:
    std::string serialize(const IInsight& insight) override {
        // Polymorphic serialization based on insight type
        if (insight.type() == "ProcessLifecycle") {
            const auto& plInsight = static_cast<const ProcessLifecycleInsight&>(insight);
            return serialize::toCompactString({plInsight});
        }
        
        if (insight.type() == "SchedulerAnalysis") {
            const auto& sInsight = static_cast<const SchedulerInsight&>(insight);
            return sInsight.toTextReport();
        }
        
        return "{}"; // Fallback for unknown types
    }
};
