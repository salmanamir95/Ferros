use crate::storage::StorageBackend;
use exec_monitor_common::SyscallEvent;
use std::fs::OpenOptions;
use std::io::Write;
use serde::Serialize;
use serde_json::json;

pub struct NdjsonStorage {
    file: std::fs::File,
}

impl NdjsonStorage {
    pub fn new(path: &str) -> anyhow::Result<Self> {
        let file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(path)?;
        Ok(Self { file })
    }
}

impl StorageBackend for NdjsonStorage {
    fn store(&mut self, event: &SyscallEvent) -> anyhow::Result<()> {
        let comm_str = core::str::from_utf8(&event.comm)
            .unwrap_or("")
            .trim_end_matches('\0');

        let wall_clock_time = chrono::Utc::now().to_rfc3339();

        let json_event = json!({
            "boot_time_ns": event.boot_time_ns,
            "wall_clock_time": wall_clock_time,
            "pid": event.pid,
            "ppid": event.ppid,
            "uid": event.uid,
            "gid": event.gid,
            "syscall_id": event.syscall_id,
            "comm": comm_str,
            "cpu_usage_snapshot": event.cpu_usage_snapshot,
            "memory_rss_snapshot": event.memory_rss_snapshot,
            "network_tx": event.network_tx,
            "network_rx": event.network_rx,
            "temperature": event.temperature,
            "power_mw": event.power_mw,
            "threat_score": event.threat_score,
        });

        writeln!(self.file, "{}", json_event)?;
        Ok(())
    }
}
