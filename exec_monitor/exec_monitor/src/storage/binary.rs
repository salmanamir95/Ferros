use crate::storage::StorageBackend;
use exec_monitor_common::ExecEvent;
use std::fs::{File, OpenOptions};
use std::io::Write;

pub struct BinaryStorage {
    file: File,
}

impl BinaryStorage {
    pub fn new(path: &str) -> anyhow::Result<Self> {
        let file = OpenOptions::new()
            .create(true)
            .append(true)
            .open(path)?;
        Ok(Self { file })
    }
}

impl StorageBackend for BinaryStorage {
    fn store(&mut self, event: &ExecEvent) -> anyhow::Result<()> {
        let bytes: [u8; std::mem::size_of::<ExecEvent>()] = unsafe { std::mem::transmute_copy(event) };
        self.file.write_all(&bytes)?;
        self.file.flush()?;
        Ok(())
    }
}
