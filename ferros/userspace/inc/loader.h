#ifndef LOADER_H
#define LOADER_H

#include "mods/CPUTelemetry.h"
// Starts the eBPF lifecycle and returns 0 on success, 1 on failure.
int start_ebpf(CPUTelemetry &telemetry);

#endif