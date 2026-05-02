use crate::storage::StorageBackend;
use csv::Writer;
use exec_monitor_common::ExecEvent;
use log::info;
use std::fs::{File, OpenOptions};

pub struct CsvStorage {
    wtr: Writer<File>,
}

impl CsvStorage {
    pub fn new(path: &str) -> anyhow::Result<Self> {
        let file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(path)?;
        let wtr = csv::WriterBuilder::new().has_headers(false).from_writer(file);
        Ok(Self { wtr })
    }
}

impl StorageBackend for CsvStorage {
    fn store(&mut self, event: &ExecEvent) -> anyhow::Result<()> {
        let mut len = 0;
        while len < 16 && event.comm[len] != 0 {
            len += 1;
        }
        let comm_str = core::str::from_utf8(&event.comm[..len]).unwrap_or("unknown");
        let timestamp = chrono::Local::now().to_rfc3339();

        self.wtr.write_record(&[&timestamp, &event.pid.to_string(), comm_str])?;
        self.wtr.flush()?;
        
        info!("Saved Telemetry: PID {} executed {}", event.pid, comm_str);
        Ok(())
    }
}
