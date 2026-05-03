# FERROS: High-Performance eBPF-Powered IoT Behavioral Observability

<div align="center">
  <img src="https://img.shields.io/badge/Status-Development-orange?style=for-the-badge" alt="Status">
  <img src="https://img.shields.io/badge/Platform-Linux-blue?style=for-the-badge&logo=linux" alt="Platform">
  <img src="https://img.shields.io/badge/Language-C%2B%2B17-green?style=for-the-badge&logo=c%2B%2B" alt="Language">
  <img src="https://img.shields.io/badge/Kernel-eBPF-purple?style=for-the-badge" alt="Kernel">
</div>

---

## 🚀 Overview

**FERROS** (Linux-Native Real-Time IoT Telemetry) is a high-fidelity observability and behavioral monitoring system designed for the modern IoT edge. By leveraging **eBPF (Extended Berkeley Packet Filter)**, FERROS captures deep kernel-level events—such as process lifecycles, CPU scheduling, and syscall activity—with near-zero overhead and maximum precision.

Conventional monitoring tools often rely on polling `/proc` or `/sys`, which can miss transient events and introduce significant latency. FERROS closes this gap by using event-driven kernel probes that stream data through lock-free ring buffers directly into a modular C++ userspace engine.

## ✨ Key Features

- **Kernel-Level Visibility**: Deep inspection of system activity via eBPF tracepoints (`sched_switch`, `sched_process_exec`).
- **Ultra-Low Overhead**: Lock-free **BPF Ring Buffers** ensure high-speed kernel-to-userspace data transfer without blocking kernel execution.
- **Real-Time Aggregation**: Modular telemetry pipeline for 1s/5s/10s time-windowed statistics and feature extraction.
- **Type-Safe Contract**: A shared header layer ensuring structural consistency between kernel-space BPF and userspace C++.
- **IoT-Optimized**: Lightweight footprint suitable for Raspberry Pi, NVIDIA Jetson, and Ubuntu-based edge nodes.
- **ML-Ready Ingestion**: Structured telemetry output designed for real-time anomaly detection and behavioral profiling.

## 🏗 Architecture

FERROS is built with a strictly decoupled 3-layer architecture:

1.  **Kernel Space (eBPF)**: High-performance probes written in C, compiled to BPF bytecode, and attached to kernel tracepoints. These probes handle raw event capture and filtering.
2.  **Shared Layer**: A header-only library providing shared constants, event structures, and type definitions used by both the kernel and userspace.
3.  **Userspace (C++ Engine)**: A multi-threaded core responsible for:
    *   Loading eBPF programs via `libbpf`.
    *   Ingesting events from the Ring Buffer.
    *   Enriching raw events with system metadata (PID, Comm, CPU stats).
    *   Aggregating metrics for analysis and logging.

## 🛠 Prerequisites

### System Requirements
*   **Linux Kernel**: 5.8 or higher (required for BPF Ring Buffer support).
*   **Architecture**: x86_64 or AArch64 (ARMv8).

### Build Dependencies
```bash
# Install dependencies on Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y clang llvm libbpf-dev libelf-dev zlib1g-dev cmake build-essential
```

## 🔨 Build & Install

FERROS uses a unified CMake build system that handles eBPF skeleton generation automatically.

```bash
# Clone the repository
git clone https://github.com/salmanamir95/Ferros.git
cd Ferros/ferros

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)
```

## 🚦 Usage

Loading eBPF programs and attaching to kernel tracepoints requires **root privileges**.

```bash
# Run the FERROS engine
sudo ./ferros
```

### Configuration
FERROS can be configured via environment variables or CLI flags (coming soon). Currently, it defaults to monitoring all scheduling events and aggregating them into the core telemetry bundle.

## 📂 Project Structure

```text
.
├── ferros/
│   ├── ebpf/          # Kernel-space BPF source code
│   │   ├── src/       # .bpf.c probe implementations
│   │   └── inc/       # BPF-specific internal headers
│   ├── userspace/     # C++ userspace engine
│   │   ├── src/       # Core logic, telemetry, and mods
│   │   └── inc/       # Public headers and BPF skeletons
│   ├── shared/        # Shared contract layer (headers)
│   └── cmake/         # CMake build automation modules
├── docs/              # Research reports and architecture diagrams
└── README.md          # Project documentation
```

## 🤝 Team & Attribution

*   **Institution**: FAST NUCES - IoT Systems Lab
*   **Lead Developer**: Muhammad Salman Amir ([@salmanamir95](https://github.com/salmanamir95))
*   **Department**: Computer Science (IoT & Systems)
*   **Project Context**: This project was developed as part of the IoT Semester Project (2026), focusing on kernel-native observability and autonomous response.

## 📄 License

This project is licensed under the **GPL-2.0** (for eBPF kernel components) and **MIT** (for userspace components). See the individual file headers for details.

---

<div align="center">
  <b>FERROS Systems Lab</b><br>
  <i>"Closing the gap between kernel-level observability and IoT operational intelligence."</i>
</div>
