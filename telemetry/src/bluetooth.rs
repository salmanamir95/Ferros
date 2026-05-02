use std::fs;
use crate::events::Dispatcher;

pub struct BluetoothCollector;

impl BluetoothCollector {
    pub fn new() -> Self {
        Self
    }

    pub fn collect(&self, dispatcher: &Dispatcher) {
        let devices = Self::list_bluetooth_devices();
        dispatcher.send(crate::events::TelemetryEvent::BluetoothEvent { devices });
    }

    fn list_bluetooth_devices() -> Vec<String> {
        let mut devices = Vec::new();
        // Simple: list /sys/class/bluetooth if exists
        if let Ok(entries) = fs::read_dir("/sys/class/bluetooth") {
            for entry in entries.filter_map(|e| e.ok()) {
                let name = entry.file_name().to_string_lossy().to_string();
                devices.push(name);
            }
        }
        // Or /dev/hci*
        if let Ok(dev_entries) = fs::read_dir("/dev") {
            for entry in dev_entries.filter_map(|e| e.ok()) {
                let name = entry.file_name().to_string_lossy().to_string();
                if name.starts_with("hci") {
                    devices.push(format!("/dev/{}", name));
                }
            }
        }
        devices
    }
}