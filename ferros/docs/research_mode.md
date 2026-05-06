# 🧪 Dataset Generation Mode (Research)

This mode is used to generate the **Synthetic OS Behavior Dataset (v1.0)**. It automates the execution of controlled workloads and performs auto-labeling using a 100ms sliding window model.

## 🚀 How to Run

### 1. Compile the Research Suite
You must compile both the workload generators and the labeling engine:
```bash
make -C research/workloads
make -C research/labeler
```

### 2. Execute the Dataset Orchestrator
The orchestrator manages the entire lifecycle (clean -> execute -> label):
```bash
python3 research/runner/run_dataset.py
```

## 📦 Output Structure
All research artifacts are stored in the `datasets/` root:
- **/telemetry**: Raw high-resolution JSON files.
- **/labels**: ML-ready feature vectors with `adversarial_behavior_analog_label`.
- **/metadata.json**: The master schema and environment spec.

## 🏷️ Labeling Logic
Labels are assigned based on a **Dual-Layer Inference Engine**:
1. **Behavioral Layer**: Technical classification (e.g., `cpu_bursty`).
2. **Adversarial Analog Layer**: Research-grade interpretation (e.g., `bursty_resource_spike_behavior`).

Check the `decision_trace` field in the label JSONs to see the exact logic triggers for each classification.
