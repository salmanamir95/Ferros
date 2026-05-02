use std::fs;
use crate::events::Dispatcher;

pub struct PciCollector;

impl PciCollector {
    pub fn new() -> Self {
        Self
    }

    pub fn collect(&self, dispatcher: &Dispatcher) {
        let devices = Self::list_pci_devices();
        dispatcher.send(crate::events::TelemetryEvent::PciEvent { devices });
    }

    fn list_pci_devices() -> Vec<String> {
        let mut devices = Vec::new();
        if let Ok(entries) = fs::read_dir("/sys/bus/pci/devices") {
            for entry in entries.filter_map(|e| e.ok()) {
                let path = entry.path();
                let name = path.file_name().unwrap_or_default().to_string_lossy().to_string();
                devices.push(name);
            }
        }
        devices
    }
}