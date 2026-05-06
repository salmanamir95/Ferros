#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "common/TelemetryType.h"
#include "common/IInsight.h"

// Forward declaration
class TelemetryBundle;

/**
 * @brief Base analyzer interface.
 * Analyzers process telemetry bundles and produce insights.
 */
class IAnalyzer
{
public:
    virtual ~IAnalyzer() = default;

    /**
     * @brief Processes the telemetry bundle and populates the insights vector.
     * @param bundle The telemetry data to analyze.
     * @param insights Vector to be populated with new insights.
     */
    virtual void run(const TelemetryBundle& bundle, std::vector<std::shared_ptr<IInsight>>& insights) = 0;
};
