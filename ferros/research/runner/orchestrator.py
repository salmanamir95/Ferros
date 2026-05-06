import os
import subprocess
import time
import json
import glob
import shutil

# FERROS RESEARCH ORCHESTRATOR
# Unified runner for the consolidated Bayesian research pipeline.

TELEMETRY_DIR = "JSON"
DATASET_ROOT = "datasets/v2"
LABELER_BIN = "build_research/labeler"
SIMULATOR_BIN = "build_research/simulator"
TRANSITION_LOG = "research/transition_log.json"
FERROS_BIN = "build/ferros"

def clean_env():
    # Use sudo to clean root-owned telemetry
    subprocess.run(f"sudo rm -f {TELEMETRY_DIR}/*.json", shell=True)
    os.makedirs(f"{DATASET_ROOT}/training/telemetry", exist_ok=True)
    os.makedirs(f"{DATASET_ROOT}/training/labels", exist_ok=True)
    os.makedirs(f"{DATASET_ROOT}/validation/telemetry", exist_ok=True)
    os.makedirs(f"{DATASET_ROOT}/validation/labels", exist_ok=True)

def run_experiment(session_type, run_id):
    print(f"\n🧪 [{session_type.upper()}] Run {run_id}...")
    subprocess.run(f"sudo rm -f {TELEMETRY_DIR}/*.json", shell=True)
    
    # 1. Start Streamer
    ferros = subprocess.Popen(["sudo", f"./{FERROS_BIN}"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(2)
    
    # 2. Execute Simulator
    subprocess.run([f"./{SIMULATOR_BIN}"], check=True)
    
    # 3. Stop Streamer
    subprocess.run(["sudo", "pkill", "-SIGINT", "ferros"])
    ferros.wait()
    time.sleep(1)
    
    # 4. Find Telemetry (Exclude manifests)
    telemetry_files = [f for f in glob.glob(f"{TELEMETRY_DIR}/*.json") if "index.json" not in f and "latest.json" not in f]
    if not telemetry_files: return
    latest_t = max(telemetry_files, key=os.path.getctime)
    
    dest_t = f"{DATASET_ROOT}/{session_type}/telemetry/run_{run_id}.json"
    dest_l = f"{DATASET_ROOT}/{session_type}/labels/run_{run_id}.json"
    shutil.copy(latest_t, dest_t)
    
    # 5. Bayesian Labeling
    print(f"🧠 Computing Bayesian Posteriors...")
    subprocess.run([f"./{LABELER_BIN}", dest_t, TRANSITION_LOG, dest_l], check=True)
    print(f"✅ Run {run_id} complete.")

def main():
    print("=== Ferros Research Orchestrator (V2) ===")
    
    # 1. Self-Healing Build: Ensure research binaries exist
    if not os.path.exists(SIMULATOR_BIN) or not os.path.exists(LABELER_BIN):
        print("🔨 Research binaries missing. Compiling suite...")
        subprocess.run(["make", "-C", "research"], check=True)

    if not os.path.exists(FERROS_BIN):
        print(f"❌ Error: {FERROS_BIN} not found. Build the core project first.")
        return
    
    clean_env()
    
    # Balanced Dataset Generation
    run_experiment("training", 0)
    run_experiment("validation", 0)
    
    print("\n🏁 Research Pipeline Complete.")

if __name__ == "__main__":
    main()
