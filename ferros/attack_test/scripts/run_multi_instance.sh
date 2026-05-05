#!/bin/bash

# run_multi_core.sh - Spawns multiple instances of the stress binary
# Optionally pins to different CPU cores using taskset

INSTANCES=${1:-2}
CORES_PER_INSTANCE=${2:-1}

echo "🔥 Spawning $INSTANCES instances of cpu_stress ($CORES_PER_INSTANCE threads each)..."

# Build if binary doesn't exist
if [ ! -f "./build/cpu_stress" ]; then
    echo "⚙️ Building cpu_stress..."
    mkdir -p build
    cd build && cmake .. && make -j$(nproc)
    cd ..
fi

for i in $(seq 0 $((INSTANCES-1))); do
    CORE=$((i % $(nproc)))
    echo "🚀 Starting instance $i (pinned to core $CORE)..."
    # Use taskset if available to pin to a specific core
    if command -v taskset >/dev/null 2>&1; then
        taskset -c $CORE ./build/cpu_stress --mode mixed --threads $CORES_PER_INSTANCE --tag "instance-$i" > /dev/null 2>&1 &
    else
        ./build/cpu_stress --mode mixed --threads $CORES_PER_INSTANCE --tag "instance-$i" > /dev/null 2>&1 &
    fi
done

echo "✅ Instances launched. Use stop_stress.sh to kill them."
