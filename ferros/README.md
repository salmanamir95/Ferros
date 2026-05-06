# 🚀 Ferros: High-Performance Linux Observability & Research Framework

Ferros is a kernel-grade observability pipeline designed for high-resolution scheduling analysis and synthetic behavioral research.

## 🌟 Key Features
- **Deterministic Replay**: Reconstruct precise CPU execution slices from eBPF truth streams.
- **Two-Tier Intelligence**: Automated inference of system pressure, starvation, and behavioral anomalies.
- **Research Pipeline**: Automated generation of ground-truth datasets for ML and OS research.

---

## 🛠️ Operational Modes

Ferros supports two primary modes of operation. Please refer to the documentation for detailed instructions:

### 🛰️ [Observability Mode](docs/observability_mode.md)
*Passive, real-time monitoring and telemetry.*
- **Command**: `sudo ./build/ferros`
- **Output**: Detailed behavioral JSON telemetry.

### 🧪 [Dataset Generation Mode](docs/research_mode.md)
*Active workload simulation and ground-truth labeling.*
- **Command**: `python3 research/runner/run_dataset.py`
- **Output**: ML-ready labeled datasets with adversarial analogs.

### 🧠 [Bayesian Behavioral Framework](docs/research_mode.md)
*Probabilistic classification and temporal transition research.*
- **Command**: `python3 research/runner/run_framework.py`
- **Output**: V2.0 dataset with Mixture Ratios, Posteriors, and Entropy metrics.

---

## 📁 Repository Structure
- **/ebpf**: Kernel-side tracepoint handlers.
- **/userspace**: Core C++ analysis pipeline and orchestrator.
- **/research**: Workload generators and labeling engine.
- **/datasets**: Research artifacts and metadata.
- **/docs**: Comprehensive guides for each mode.

---

## 📜 License
Ferros Research Edition. For academic and professional observability research.
