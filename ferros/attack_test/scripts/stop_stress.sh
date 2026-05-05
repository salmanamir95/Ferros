#!/bin/bash

# stop_stress.sh - Kills all running stress test processes safely

echo "🛑 Stopping all cpu_stress processes..."

# Find all pids of cpu_stress
PIDS=$(pgrep cpu_stress)

if [ -z "$PIDS" ]; then
    echo "ℹ️ No cpu_stress processes found."
else
    echo "🔪 Killing PIDs: $PIDS"
    kill -SIGINT $PIDS
    sleep 1
    # Check if still running, then force kill
    PIDS_REMAINING=$(pgrep cpu_stress)
    if [ ! -z "$PIDS_REMAINING" ]; then
        echo "⚠️ Some processes didn't stop, force killing..."
        kill -9 $PIDS_REMAINING
    fi
    echo "✅ All stress processes stopped."
fi
