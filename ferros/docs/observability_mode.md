# 🛰️ Observability Mode

Observability Mode is the core heartbeat of Ferros. It leverages eBPF tracepoints (`sched_switch`, `sched_fork`) to reconstruct the absolute truth of the Linux scheduler in userspace.

## 🚀 How to Run

### 1. Build the Core
Ensure the C++ pipeline and eBPF programs are compiled:
```bash
cmake -S . -B build
cmake --build build
```

### 2. Launch the Streamer
Ferros requires `sudo` to attach eBPF probes to the kernel:
```bash
sudo ./build/ferros
```

## 📊 Telemetry Output
By default, Ferros writes telemetry to the `JSON/` directory. Each file follows a collection cycle:
- **Deterministic Analysis**: Process lifetimes, CPU slices, and migration history.
- **Behavioral Insights**: Inferred patterns like CPU Starvation or Preemption Dominance.

## 🛠️ Configuration
You can modify the analysis thresholds in:
`userspace/src/analyzer/cpu/SchedulerAnalyzer.cpp`
