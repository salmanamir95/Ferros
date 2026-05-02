pub mod ndjson;
pub mod multiplexer;

use exec_monitor_common::SyscallEvent;

/// Dependency Inversion: Storage logic is abstracted behind this trait.
pub trait StorageBackend: Send + 'static {
    fn store(&mut self, event: &SyscallEvent) -> anyhow::Result<()>;
}
