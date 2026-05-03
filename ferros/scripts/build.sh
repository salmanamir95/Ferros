#!/usr/bin/env bash

set -e  # stop immediately if anything fails

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build_final"

echo "== Ferros Build Script =="

# Step 1: go to project root (just in case)
cd "$PROJECT_DIR"

# Step 2: clean old build
echo "[1/5] Cleaning build directory..."
rm -rf "$BUILD_DIR" || sudo rm -rf "$BUILD_DIR" || true

# Step 3: recreate build directory
echo "[2/5] Creating build directory..."
mkdir -p "$BUILD_DIR"

# Step 4: configure project
echo "[3/5] Running CMake..."
cd "$BUILD_DIR"
cmake ..

# Step 5: compile
echo "[4/5] Building project..."
make -j"$(nproc)"

echo "[5/5] Build complete."

# Step 6: run (ask for sudo only here)
echo ""
read -p "Run Ferros with sudo now? (y/n): " choice

if [[ "$choice" == "y" || "$choice" == "Y" ]]; then
    echo "Starting Ferros..."
    sudo ./ferros
else
    echo "Skipping execution."
fi