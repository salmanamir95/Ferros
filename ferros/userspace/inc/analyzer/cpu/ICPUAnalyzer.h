#pragma once

#include "analyzer/IAnalyzer.h"
#include "telemetry/TelemetryBundle.h"

// CPU domain analyzer (orchestrator layer)
class ICPUAnalyzer : public IAnalyzer
{
public:
    virtual ~ICPUAnalyzer() = default;

    // Entry point from AnalyzerRegistry
    void run(const TelemetryBundle& bundle) override
    {
        analyze(bundle);
    }

    // CPU-specific analysis entry
    virtual void analyze(const TelemetryBundle& bundle) = 0;

    // Optional: allows plugging CPU sub-analyzers
    //virtual void addAnalyzer(IAnalyzer* analyzer) = 0;
};