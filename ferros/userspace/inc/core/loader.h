#ifndef LOADER_H
#define LOADER_H

#include "telemetry/TelemetryBundle.h"
#include "analyzer/AnalyzerRegistry.h"
// Starts the eBPF lifecycle and returns 0 on success, non-zero on failure.
int start_ebpf(TelemetryBundle &bundle, AnalyzerRegistry &registry);

#endif