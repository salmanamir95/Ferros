use sysinfo::{System, Disks};
use crate::events::Dispatcher;

pub struct SystemCollector {
    system: System,
    disks: Disks,
}

impl SystemCollector {
    pub fn new() -> Self {
        Self {
            system: System::new_all(),
            disks: Disks::new_with_refreshed_list(),
        }
    }

    pub fn collect(&mut self, dispatcher: &Dispatcher) {
        self.system.refresh_cpu();
        self.system.refresh_memory();
        self.disks.refresh();

        // CPU
        let cpu_usage: f32 = self.system.cpus().iter().map(|cpu| cpu.cpu_usage()).sum::<f32>() / self.system.cpus().len() as f32;
        dispatcher.send(crate::events::TelemetryEvent::CpuEvent { usage: cpu_usage });

        // Memory
        let total_ram = self.system.total_memory();
        let used_ram = self.system.used_memory();
        dispatcher.send(crate::events::TelemetryEvent::MemoryEvent { total: total_ram, used: used_ram });

        // Disks
        for disk in self.disks.iter() {
            let name = disk.name().to_string_lossy().to_string();
            let total = disk.total_space();
            let available = disk.available_space();
            dispatcher.send(crate::events::TelemetryEvent::DiskEvent { name, total, available });
        }
    }
}