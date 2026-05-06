# 🛠️ Ferros Operational Modes

Ferros is designed to function in two distinct modes depending on your objective: **Real-time Observability** or **Scientific Research**.

---

## 🛰️ 1. Observability Mode
**Objective**: Continuous, high-fidelity monitoring of system behavior via eBPF.

In this mode, Ferros acts as a passive observer, streaming kernel events and performing deterministic analysis (Tier 2) and behavioral inference (Tier 3) on the fly.

> [!TIP]
> Use this mode for system debugging, performance profiling, and real-time anomaly detection.

**[Read the Observability Guide](observability_mode.md)**

---

## 🧪 2. Dataset Generation Mode (Research)
**Objective**: Automated generation of ground-truth labeled datasets for ML and OS research.

This mode uses a "Controlled-Execution" pipeline. It synchronizes Ferros with a suite of synthetic workloads, extracts windowed statistical features, and assigns **Adversarial Behavior Analog Labels**.

> [!IMPORTANT]
> This mode requires the research suite to be compiled and uses a Python orchestrator to manage the isolation cycles.

**[Read the Research Pipeline Guide](research_mode.md)**
