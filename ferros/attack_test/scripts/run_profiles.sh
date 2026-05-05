#!/bin/bash

# run_profiles.sh - Accept arguments: mode, threads, duration

MODE=${1:-mixed}
THREADS=${2:-$(nproc)}
DURATION=${3:--1}
TAG=${4:-profile-run}

echo "🔥 Launching CPU stress with profile: $MODE"
echo "🧵 Threads: $THREADS"
echo "⏱️ Duration: $DURATION"

# Build if binary doesn't exist
if [ ! -f "./build/cpu_stress" ]; then
    echo "⚙️ Building cpu_stress..."
    mkdir -p build
    cd build && cmake .. && make -j$(nproc)
    cd ..
fi

./build/cpu_stress --mode $MODE --threads $THREADS --duration $DURATION --tag $TAG
