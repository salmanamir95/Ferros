#!/bin/bash

# run_stress.sh - Launch CPU stress test with default thread count
# Default: number of CPU cores

CORES=$(nproc)
echo "🔥 Launching CPU stress test on $CORES cores..."

# Build if binary doesn't exist
if [ ! -f "./build/cpu_stress" ]; then
    echo "⚙️ Building cpu_stress..."
    mkdir -p build
    cd build && cmake .. && make -j$(nproc)
    cd ..
fi

./build/cpu_stress --mode mixed --threads $CORES
