#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "common/TelemetryType.h"

// Forward declaration
class TelemetryBundle;

// Base analyzer interface (you already have CPU-specific later)
class IAnalyzer
{
public:
    virtual ~IAnalyzer() = default;

    virtual void run(const TelemetryBundle& bundle) = 0;
};
