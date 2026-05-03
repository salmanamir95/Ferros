#!/usr/bin/env bash

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
  echo "Please run as root (sudo ./scripts/debug.sh)"
  exit 1
fi

echo "== Ferros Debug Monitor (trace_pipe) =="
echo "Watching for bpf_printk output... Press Ctrl+C to stop."
echo ""

# Clear existing trace data
echo 0 > /sys/kernel/debug/tracing/trace

# Read from trace_pipe
cat /sys/kernel/debug/tracing/trace_pipe
