use std::fs;
use evdev::enumerate;
use crate::events::Dispatcher;

pub struct DeviceCollector;

impl DeviceCollector {
    pub fn new() -> Self {
        Self
    }

    pub fn collect(&self, dispatcher: &Dispatcher) {
        // Input devices
        let input_devices_paths = Self::list_devices("/dev/input");
        dispatcher.send(crate::events::TelemetryEvent::DeviceEvent {
            category: "Input".to_string(),
            devices: input_devices_paths,
        });

        let input_devices: Vec<String> = enumerate()
            .map(|(path, device)| format!("{}: {}", path.display(), device.name().unwrap_or("Unknown")))
            .collect();
        if !input_devices.is_empty() {
            dispatcher.send(crate::events::TelemetryEvent::DeviceEvent {
                category: "Input Detailed".to_string(),
                devices: input_devices,
            });
        }

        // Audio output devices
        let audio_devices = Self::list_devices("/dev/snd");
        dispatcher.send(crate::events::TelemetryEvent::DeviceEvent {
            category: "Audio Output".to_string(),
            devices: audio_devices,
        });

        // Graphics output devices
        let graphics_devices = Self::list_devices("/dev/dri");
        dispatcher.send(crate::events::TelemetryEvent::DeviceEvent {
            category: "Graphics Output".to_string(),
            devices: graphics_devices,
        });

        // Cameras
        let cameras = Self::list_devices("/dev");
        let cameras: Vec<String> = cameras.into_iter().filter(|d| d.starts_with("/dev/video")).collect();
        dispatcher.send(crate::events::TelemetryEvent::DeviceEvent {
            category: "Cameras".to_string(),
            devices: cameras,
        });

        // Printers (simple, list USB printers)
        let printers = Self::list_devices("/dev/usb");
        let printers: Vec<String> = printers.into_iter().filter(|d| d.contains("lp")).collect();
        dispatcher.send(crate::events::TelemetryEvent::DeviceEvent {
            category: "Printers".to_string(),
            devices: printers,
        });
    }

    fn list_devices(dir: &str) -> Vec<String> {
        fs::read_dir(dir)
            .map(|entries| {
                entries.filter_map(|e| e.ok().map(|e| e.path().to_string_lossy().to_string())).collect()
            })
            .unwrap_or_default()
    }
}