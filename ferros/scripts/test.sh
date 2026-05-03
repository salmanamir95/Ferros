#!/usr/bin/env bash

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build_final"

echo "== Ferros Test Workload Generator =="

# 1. Ensure project is built
if [ ! -f "$BUILD_DIR/ferros" ]; then
    echo "Executable not found. Building first..."
    "$PROJECT_DIR/scripts/build.sh"
fi

# 2. Start Ferros in the background
echo "[1/3] Starting Ferros monitor in background..."
sudo "$BUILD_DIR/ferros" > "$BUILD_DIR/ferros_test.log" 2>&1 &
FERROS_PID=$!

# 3. Generate some system activity
echo "[2/3] Generating system workload (stressing CPUs for 5 seconds)..."
stress -c 4 -t 5

# 4. Wait a moment for events to be processed
sleep 2

# 5. Stop Ferros
echo "[3/3] Stopping Ferros..."
sudo kill -SIGINT $FERROS_PID
wait $FERROS_PID 2>/dev/null

echo ""
echo "== Test Complete =="
echo "Check $BUILD_DIR/ferros_test.log for captured events."
echo "First 10 lines of capture:"
echo "----------------------------------------"
head -n 20 "$BUILD_DIR/ferros_test.log"
echo "----------------------------------------"
