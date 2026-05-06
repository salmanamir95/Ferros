#pragma once

#include <vector>
#include "telemetry/CPUTelemetry.h"
#include "events.h"

class TelemetryBundle
{
public:
    TelemetryBundle() = default;
    ~TelemetryBundle() = default;

    // Domain access (legacy normalization)
    CPUTelemetry& cpu() { return cpuTelemetry; }
    const CPUTelemetry& cpu() const { return cpuTelemetry; }

    // Production Truth Stream access
    void addRawEvent(const foc_event& ev) { rawEvents.push_back(ev); }
    const std::vector<foc_event>& raw() const { return rawEvents; }
    void clearRaw() { rawEvents.clear(); }

private:
    CPUTelemetry cpuTelemetry;
    std::vector<foc_event> rawEvents;
};