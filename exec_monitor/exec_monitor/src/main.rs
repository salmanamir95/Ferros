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

#[cfg(target_arch = "x86_64")]
const SYSCALLS: &[u32] = &[59, 42, 257, 56, 1]; // execve, connect, openat, clone, write

#[cfg(target_arch = "aarch64")]
const SYSCALLS: &[u32] = &[221, 203, 56, 220, 64];

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    // 1. Load BPF program
    let mut app = BpfApp::load()?;
    
    // Initialize syscalls mapping
    app.init_syscalls(SYSCALLS)?;

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

    // 5. Start Storage Consumer Thread with basic enrichment
    thread::spawn(move || {
        let mut sys = System::new_all();
        while let Ok(mut event) = rx.recv() {
            sys.refresh_cpu();
            sys.refresh_memory();
            
            event.cpu_usage = sys.global_cpu_info().cpu_usage() as u32;
            event.memory_usage = sys.used_memory();

            let _ = multiplexer.store(&event);
        }
    });

    println!("Waiting for Ctrl-C...");
    signal::ctrl_c().await?;
    println!("Exiting...");

    Ok(())
}