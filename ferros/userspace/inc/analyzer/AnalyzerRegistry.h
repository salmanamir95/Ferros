#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "common/TelemetryType.h"
#include "analyzer/IAnalyzer.h"
// Forward declaration
class TelemetryBundle;

// Base analyzer interface (you already have CPU-specific later)
class IAnalyzer;

class AnalyzerRegistry
{
public:
    void registerAnalyzer(
        TelemetryType type,
        IAnalyzer* analyzer
    )
    {
        registry[type].push_back(analyzer);
    }

    void run(
        TelemetryType type,
        const TelemetryBundle& bundle
    ) const
    {
        auto it = registry.find(type);
        if (it == registry.end())
            return;

        for (auto* analyzer : it->second)
        {
            analyzer->run(bundle);
        }
    }

    void runAll(const TelemetryBundle& bundle) const
    {
        for (auto& [type, list] : registry)
        {
            for (auto* analyzer : list)
            {
                analyzer->run(bundle);
            }
        }
    }

private:
    std::unordered_map<TelemetryType, std::vector<IAnalyzer*>> registry;
};