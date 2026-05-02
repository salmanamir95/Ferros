mod bpf;
mod poller;
mod storage;

use std::sync::mpsc;
use std::thread;
use tokio::signal;

use bpf::BpfApp;
use exec_monitor_common::SyscallEvent;
use poller::EventPoller;
use storage::{ndjson::NdjsonStorage, multiplexer::StorageMultiplexer, StorageBackend};
use sysinfo::System;

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    // 1. Load BPF program
    let mut app = BpfApp::load()?;

    // 2. Set up Storage Multiplexer
    let mut multiplexer = StorageMultiplexer::new();
    multiplexer.add_backend(NdjsonStorage::new("telemetry.ndjson")?);

    // 3. Set up multi-producer, single-consumer channel
    let (tx, rx) = mpsc::channel::<SyscallEvent>();

    // 4. Start Event Poller
    let perf_array = aya::maps::perf::PerfEventArray::try_from(app.ebpf.take_map("EVENTS").unwrap())?;
    let poller = EventPoller::new(perf_array);
    poller.start(tx)?;

    // Start userspace telemetry polling in background
    tokio::spawn(async move {
        if let Err(e) = telemetry::start_telemetry().await {
            log::error!("Telemetry error: {}", e);
        }
    });

    // 5. Start Storage Consumer Thread with accurate enrichment
    thread::spawn(move || {
        let mut sys = System::new_all();
        while let Ok(mut event) = rx.recv() {
            let pid = sysinfo::Pid::from_u32(event.pid);
            
            // Refresh specific process
            sys.refresh_process(pid);
            
            if let Some(process) = sys.process(pid) {
                // PPID Fallback via native /proc/<pid>/stat (not relying on sysinfo internals for PPID)
                if event.ppid == 0 {
                    if let Ok(stat) = std::fs::read_to_string(format!("/proc/{}/stat", event.pid)) {
                        // Safely parse after the last ')' to avoid issues with spaces in the process name
                        if let Some(rparen) = stat.rfind(')') {
                            let remainder = &stat[rparen + 1..];
                            let parts: Vec<&str> = remainder.split_whitespace().collect();
                            // parts[0] = state, parts[1] = ppid
                            if parts.len() > 1 {
                                if let Ok(parent_pid) = parts[1].parse::<u32>() {
                                    event.ppid = parent_pid;
                                }
                            }
                        }
                    }
                }
                // Accurate Snapshot Metrics
                event.cpu_usage_snapshot = process.cpu_usage() as u32;
                event.memory_rss_snapshot = process.memory();
            } else {
                // If process is already dead, default to 0
                event.cpu_usage_snapshot = 0;
                event.memory_rss_snapshot = 0;
            }

            let _ = multiplexer.store(&event);
        }
    });

    println!("Waiting for Ctrl-C...");
    signal::ctrl_c().await?;
    println!("Exiting...");

    Ok(())
}