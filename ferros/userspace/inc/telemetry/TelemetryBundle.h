#pragma once

#include "telemetry/CPUTelemetry.h"

// later you will add:
// #include "telemetry/MemoryTelemetry.h"
// #include "telemetry/SchedTelemetry.h"

class TelemetryBundle
{
public:
    TelemetryBundle() = default;
    ~TelemetryBundle() = default;

    // ----------------------------
    // Domain access (read/write)
    // ----------------------------
    CPUTelemetry& cpu() { return cpuTelemetry; }
    const CPUTelemetry& cpu() const { return cpuTelemetry; }

    // Future extensions:
    // MemoryTelemetry& memory();
    // SchedTelemetry& sched();

private:
    CPUTelemetry cpuTelemetry;

    // Future fields:
    // MemoryTelemetry memoryTelemetry;
    // SchedTelemetry schedTelemetry;
};