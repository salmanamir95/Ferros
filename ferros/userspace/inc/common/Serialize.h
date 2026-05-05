// Serialize.h

#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "analyzer/cpu/pid_analyzers/ProcessLifecycleInsight.h"

namespace serialize
{
    // Single insight
    nlohmann::json toJson(
        const ProcessLifecycleInsight& insight);

    // Vector of insights
    nlohmann::json toJson(
        const std::vector<ProcessLifecycleInsight>& insights);

    // Convenience helpers
    std::string toPrettyString(
        const ProcessLifecycleInsight& insight);

    std::string toPrettyString(
        const std::vector<ProcessLifecycleInsight>& insights);

    std::string toCompactString(
        const std::vector<ProcessLifecycleInsight>& insights);
}