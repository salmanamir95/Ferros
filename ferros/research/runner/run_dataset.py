import os
import subprocess
import time
import signal
import json
import glob
import shutil

# RESEARCH ORCHESTRATOR
# Manages execution cycles, telemetry capture, and auto-labeling.

WORKLOADS_DIR = "research/workloads"
LABELER_BIN = "research/labeler/LabelEngine"
FERROS_BIN = "./build/ferros"
TELEMETRY_DIR = "JSON"
DATASET_DIR = "datasets"

WORKLOADS = [
    "baseline",
    "sustained_load",
    "burst_load",
    "scheduler_stress",
    "process_growth",
    "migration_evasion"
]

def clean_telemetry():
    files = glob.glob(f"{TELEMETRY_DIR}/*.json")
    for f in files:
        os.remove(f)

def get_latest_telemetry():
    files = glob.glob(f"{TELEMETRY_DIR}/*.json")
    if not files:
        return None
    return max(files, key=os.path.getctime)

def run_experiment(workload_name):
    print(f"\n🚀 Running Experiment: {workload_name}")
    
    # 1. Clean previous runs
    clean_telemetry()
    
    # 2. Start Ferros (eBPF Pipeline)
    # Using sudo for eBPF attachment
    ferros_proc = subprocess.Popen(["sudo", FERROS_BIN], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(2) # Give it a moment to attach
    
    # 3. Execute Workload
    workload_path = os.path.join(WORKLOADS_DIR, workload_name)
    try:
        subprocess.run([workload_path], check=True, timeout=45)
    except subprocess.TimeoutExpired:
        print(f"⚠️ Workload {workload_name} timed out.")
    
    # 4. Stop Ferros
    # We use sudo kill because it was started with sudo
    subprocess.run(["sudo", "pkill", "-SIGINT", "ferros"])
    ferros_proc.wait()
    
    # 5. Capture Telemetry
    time.sleep(1) # Wait for file write flush
    telemetry_file = get_latest_telemetry()
    if not telemetry_file:
        print("❌ Error: No telemetry captured!")
        return

    run_id = int(time.time())
    dest_telemetry = os.path.join(DATASET_DIR, "telemetry", f"run_{workload_name}_{run_id}.json")
    shutil.copy(telemetry_file, dest_telemetry)
    
    # 6. Labeling
    meta_path = f"{WORKLOADS_DIR}/{workload_name}.meta"
    label_path = os.path.join(DATASET_DIR, "labels", f"run_{workload_name}_{run_id}.json")
    
    print(f"🏷️  Labeling run...")
    subprocess.run([LABELER_BIN, dest_telemetry, meta_path, label_path], check=True)
    
    print(f"✅ Run Complete. Telemetry: {dest_telemetry}, Labels: {label_path}")

def main():
    print("=== Ferros Research Dataset Generation Suite ===")
    
    if not os.path.exists(FERROS_BIN):
        print(f"❌ Error: {FERROS_BIN} not found. Build the project first.")
        return

    for workload in WORKLOADS:
        run_experiment(workload)

    print("\n🏁 Dataset Generation Finished.")

if __name__ == "__main__":
    main()
