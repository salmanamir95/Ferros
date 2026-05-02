pub mod binary;
pub mod csv;
pub mod multiplexer;

use exec_monitor_common::ExecEvent;

/// Dependency Inversion: Storage logic is abstracted behind this trait.
pub trait StorageBackend: Send + 'static {
    fn store(&mut self, event: &ExecEvent) -> anyhow::Result<()>;
}
