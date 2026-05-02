use std::fs;
use crate::events::Dispatcher;

pub struct SensorCollector;

impl SensorCollector {
    pub fn new() -> Self {
        Self
    }

    pub fn collect(&self, dispatcher: &Dispatcher) {
        // Temperature sensors
        if let Ok(hwmon_dirs) = fs::read_dir("/sys/class/hwmon") {
            for hwmon in hwmon_dirs.filter_map(|e| e.ok()) {
                let path = hwmon.path();
                if let Ok(temp_files) = fs::read_dir(&path) {
                    for temp_file in temp_files.filter_map(|e| e.ok()) {
                        let file_name = temp_file.file_name().to_string_lossy().to_string();
                        if file_name.starts_with("temp") && file_name.ends_with("_input") {
                            if let Ok(content) = fs::read_to_string(temp_file.path()) {
                                if let Ok(value) = content.trim().parse::<f64>() {
                                    let temp_celsius = value / 1000.0;
                                    dispatcher.send(crate::events::TelemetryEvent::SensorEvent {
                                        name: format!("{}/{}", path.display(), file_name),
                                        value: temp_celsius,
                                    });
                                }
                            }
                        }
                    }
                }
            }
        }

        // Power systems
        if let Ok(power_dirs) = fs::read_dir("/sys/class/power_supply") {
            for power in power_dirs.filter_map(|e| e.ok()) {
                let path = power.path();
                let capacity_path = path.join("capacity");
                if let Ok(content) = fs::read_to_string(&capacity_path) {
                    if let Ok(capacity) = content.trim().parse::<f64>() {
                        dispatcher.send(crate::events::TelemetryEvent::SensorEvent {
                            name: format!("{}/capacity", path.display()),
                            value: capacity,
                        });
                    }
                }
            }
        }
    }
}