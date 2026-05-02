use std::fs;
use crate::events::Dispatcher;

pub struct UsbCollector;

impl UsbCollector {
    pub fn new() -> Self {
        Self
    }

    pub fn collect(&self, dispatcher: &Dispatcher) {
        let devices = Self::list_usb_devices();
        dispatcher.send(crate::events::TelemetryEvent::UsbEvent { devices });
    }

    fn list_usb_devices() -> Vec<String> {
        let mut devices = Vec::new();
        if let Ok(entries) = fs::read_dir("/sys/bus/usb/devices") {
            for entry in entries.filter_map(|e| e.ok()) {
                let path = entry.path();
                let name = path.file_name().unwrap_or_default().to_string_lossy().to_string();
                if name.starts_with("usb") || name.contains('-') {
                    devices.push(name);
                }
            }
        }
        devices
    }
}