#pragma once

#include "analyzer/IAnalyzer.h"
#include "telemetry/TelemetryBundle.h"

/**
 * @brief CPU domain analyzer interface.
 * Specializes IAnalyzer for CPU-specific telemetry.
 */
class ICPUAnalyzer : public IAnalyzer
{
public:
    virtual ~ICPUAnalyzer() = default;

    /**
     * @brief Implements the base run method by calling the specialized analyze method.
     */
    void run(const TelemetryBundle& bundle, std::vector<std::shared_ptr<IInsight>>& insights) override
    {
        analyze(bundle);
        
        // After analysis, collect insights from this specific analyzer
        collectInsights(insights);
    }

    /**
     * @brief Specialized CPU analysis entry.
     */
    virtual void analyze(const TelemetryBundle& bundle) = 0;

    /**
     * @brief Collects insights produced during analysis.
     * @param insights Vector to be populated with new insights.
     */
    virtual void collectInsights(std::vector<std::shared_ptr<IInsight>>& insights) = 0;
};