mod bpf;
mod poller;
mod storage;

use std::sync::mpsc;
use std::thread;
use tokio::signal;

use bpf::BpfApp;
use exec_monitor_common::ExecEvent;
use poller::EventPoller;
use storage::{binary::BinaryStorage, csv::CsvStorage, multiplexer::StorageMultiplexer, StorageBackend};

#[tokio::main]
async fn main() -> anyhow::Result<()> {
    env_logger::Builder::from_env(env_logger::Env::default().default_filter_or("info")).init();

    // 1. Load BPF program
    let mut app = BpfApp::load()?;

    // 2. Set up Storage Multiplexer
    let mut multiplexer = StorageMultiplexer::new();
    multiplexer.add_backend(BinaryStorage::new("telemetry.bin")?);
    multiplexer.add_backend(CsvStorage::new("telemetry.csv")?);

    // 3. Set up multi-producer, single-consumer channel
    let (tx, rx) = mpsc::channel::<ExecEvent>();

    // 4. Start Event Poller
    let perf_array = aya::maps::perf::PerfEventArray::try_from(app.ebpf.take_map("EVENTS").unwrap())?;
    let poller = EventPoller::new(perf_array);
    poller.start(tx)?;

    // 5. Start Storage Consumer Thread
    thread::spawn(move || {
        while let Ok(event) = rx.recv() {
            let _ = multiplexer.store(&event);
        }
    });

    println!("Waiting for Ctrl-C...");
    signal::ctrl_c().await?;
    println!("Exiting...");

    Ok(())
}