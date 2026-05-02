# Secure IoT Telemetry - Codebase Analysis

The Ferros Telemetry and Behavioral Monitoring system is designed as a lightweight, production-grade observability platform for Linux-based IoT nodes. It merges deep eBPF-based kernel telemetry with userspace hardware monitoring.

This document outlines the current architectural components, what each directory manages, and the responsibility of each file within the project.

---

## Directory Overview

The project is broadly split into two overarching workspaces:

1. **`exec_monitor/`**: The core observability daemon. It handles loading the eBPF programs into the kernel, receiving telemetry through ring buffers, enriching events, and saving the data to disk.
2. **`telemetry/`**: A modular hardware and system sampling library. It abstracts the collection of hardware metrics (temperature, power, networking, system load, USB/PCI events) so they can be injected into the central event stream.
3. **`docs/`**: Project documentation, analyses, and implementation plans.

---

## `exec_monitor/` - The eBPF Observability Daemon

This is the primary binary that runs on the node (as `root`). It consists of three crates to safely bridge kernel-space and userspace.

### 1. `exec_monitor-common/`
This crate contains the shared memory structures that both the eBPF kernel program and the Rust userspace program must agree upon.
- **`src/lib.rs`**: Defines the frozen schema `SyscallEvent`. This struct represents the unified data format containing process execution data (PID, PPID, UID, Comm, Syscall ID, Timestamps) alongside hardware context fields (CPU, Memory, Networking, Thermal, Power, and Threat Score). It is the source of truth for the entire pipeline.

### 2. `exec_monitor-ebpf/`
This is the `no_std` kernel-space eBPF program compiled to bytecode and injected into the Linux kernel.
- **`src/main.rs`**: The core eBPF tracing logic. It implements five highly-optimized, dedicated tracepoints (`sys_enter_execve`, `sys_enter_connect`, `sys_enter_openat`, `sys_enter_clone`, `sys_enter_write`). Upon intercepting a syscall, it immediately grabs the process context (PID, UID, Comm, PPID), populates the kernel-side fields of `SyscallEvent`, and emits the event over a `PerfEventArray` (Ring Buffer) back to userspace with near-zero overhead.

### 3. `exec_monitor/`
This is the Rust userspace loader, poller, and normalizer.
- **`src/main.rs`**: The central entry point. It initializes the logging environment, loads the BPF application into the kernel, establishes the NDJSON storage multiplexer, and wires the internal channels. It also runs the background enrichment loop, merging real-time `sysinfo` (CPU/Memory usage) into the raw `SyscallEvent`s before dispatching them to storage.
- **`src/bpf.rs`**: The loader abstraction. It handles raising RLIMITs (memory locks) necessary for eBPF and binds the specific tracepoints from `exec_monitor-ebpf` to their corresponding Linux kernel hooks.
- **`src/poller.rs`**: The asynchronous event listener. It continually polls the `PerfEventArray` exposed by the eBPF program, safely reading raw byte arrays and transmuting them back into structured `SyscallEvent` structs, then passing them down a channel for enrichment.
- **`src/storage/mod.rs`**: The storage interface defining the `StorageBackend` trait, ensuring that telemetry serialization logic is kept entirely decoupled from collection logic.
- **`src/storage/ndjson.rs`**: The implementation of the SIEM-compatible NDJSON logger. It converts the populated `SyscallEvent` into structured JSON strings and performs append-only writes to `telemetry.ndjson`.
- **`src/storage/multiplexer.rs`**: A broadcast system allowing events to be written to multiple storage backends simultaneously (e.g., if we wanted to mirror to a remote socket and a local file).

---

## `telemetry/` - Hardware & System Sampling Library

This crate serves as a rich userspace polling library designed to run on resource-constrained devices (like the Raspberry Pi). It operates independently of the kernel tracepoints to collect contextual node metrics.

- **`src/lib.rs`**: The library entry point. It exposes the `start_telemetry` task, instantiating the internal event dispatcher and firing up individual collectors in a 1-second polling loop.
- **`src/events.rs`**: Implements an asynchronous Publish-Subscribe dispatcher for hardware events. The `LoggerSubscriber` listens for system events and currently handles writing them out to `telemetry.log`.
- **`src/system.rs`**: Contains the `SystemCollector`, utilizing `sysinfo` to capture holistic host metrics (Host Name, OS Version, Boot Time, uptime).
- **`src/network.rs`**: The `NetworkCollector` tracks traffic anomalies, measuring packets received/transmitted, errors, and interface status.
- **`src/devices.rs`**: Provides disk usage and mounted partition metrics.
- **`src/sensors.rs`**: Houses the `SensorCollector`, capable of polling thermal zones (e.g., Raspberry Pi CPU temp or DS18B20 inputs) to detect thermal spikes associated with cryptomining or heavy processing.
- **`src/usb.rs`**: Monitors and polls attached USB devices for unauthorized peripheral connections.
- **`src/pci.rs`**: Monitors the PCI bus for deeper hardware insights.
- **`src/bluetooth.rs`**: Checks the Bluetooth stack configuration and local adapter status.

---

## Architectural Flow

The repository adheres to a strict pipeline to prevent bottlenecks:

1. **Kernel Tracing**: `exec_monitor-ebpf` captures minimal execution context on specific syscalls and pushes them to a Ring Buffer.
2. **Userspace Harvesting**: `poller.rs` retrieves these events continuously.
3. **Enrichment**: `main.rs` merges the event with dynamic data pulled from the `telemetry` or `sysinfo` systems.
4. **Storage**: The enriched event is structured as NDJSON by `storage/ndjson.rs` for downstream SIEM correlation.

---

## Data Models & Telemetry Output

### 1. `SyscallEvent` (The Unified Event Record)
Defined in `exec_monitor-common/src/lib.rs`, the `SyscallEvent` struct represents the unified payload. It combines high-speed, kernel-derived metadata (captured synchronously during syscall execution) with asynchronous, userspace-derived system metrics.
Fields include:
- **Identity & Context**: `pid`, `ppid`, `uid`, `gid`, `comm` (process name), `syscall_id`.
- **Temporal**: `boot_time_ns` (nanosecond-precision kernel timestamp).
- **Userspace Enriched**: `cpu_usage_snapshot`, `memory_rss_snapshot`, `network_tx/rx`, `temperature`, `power_mw`, and `threat_score`.

### 2. Output Formats
The system utilizes two distinct logging paths:
1. **`telemetry.ndjson`**: The primary output generated by `exec_monitor`. Every line is a complete, stringified JSON object of an enriched `SyscallEvent`. The NDJSON format (Newline Delimited JSON) is specifically chosen for high-throughput, append-only writing, and native integration with big-data aggregation tools (e.g., Elasticsearch, Splunk).
2. **`telemetry.log` (stdout/stderr)**: Managed by the `telemetry` crate's `LoggerSubscriber`, this provides human-readable logs of the discrete hardware events (e.g., thermal spikes, USB attachments, PCI enumeration).

---

## Security & Future Considerations

### Current Strengths
- **Non-blocking Architecture**: The eBPF kernel program is completely decoupled from the slower userspace enrichment logic via Perf Event Arrays.
- **Resource Constraints**: By utilizing polling intervals for hardware telemetry and eBPF for behavioral telemetry, CPU overhead remains strictly bounded.
- **Tamper Resistance**: eBPF programs run entirely in kernel space, making it impossible for a userspace rootkit to intercept or tamper with the raw system calls before they are logged.

### Future Expansions
- **Network Traffic Inspection**: Deep Packet Inspection (DPI) or XDP-based network monitoring to track anomalous traffic directly from the NIC.
- **Advanced Threat Scoring**: Implementing a rules engine inside the Rust userspace daemon that calculates the `threat_score` based on moving averages and heuristics (e.g., "high CPU usage + unmapped memory execution + unknown network destination").
- **Remote Telemetry Transport**: Writing a network multiplexer in `storage/multiplexer.rs` to stream the NDJSON payload securely via mTLS to a central aggregator.
