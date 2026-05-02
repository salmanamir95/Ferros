use crate::storage::StorageBackend;
use exec_monitor_common::SyscallEvent;

/// Broadcasts an event to multiple storage backends
pub struct StorageMultiplexer {
    backends: Vec<Box<dyn StorageBackend>>,
}

impl StorageMultiplexer {
    pub fn new() -> Self {
        Self { backends: Vec::new() }
    }

    pub fn add_backend<B: StorageBackend + 'static>(&mut self, backend: B) {
        self.backends.push(Box::new(backend));
    }
}

impl StorageBackend for StorageMultiplexer {
    fn store(&mut self, event: &SyscallEvent) -> anyhow::Result<()> {
        for backend in &mut self.backends {
            // We ignore individual backend failures so one failing disk doesn't crash everything
            let _ = backend.store(event);
        }
        Ok(())
    }
}
