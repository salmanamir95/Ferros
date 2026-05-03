#include "mods/CPUTelemetry.h"

class ICPUAnalyzer {
public:
    virtual ~ICPUAnalyzer() = default;

    virtual void analyze(const CPUTelemetry& telemetry) = 0;
};