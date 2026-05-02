use tokio::sync::mpsc;

#[derive(Debug, Clone)]
pub enum TelemetryEvent {
    CpuEvent { usage: f32 },
    MemoryEvent { total: u64, used: u64 },
    DiskEvent { name: String, total: u64, available: u64 },
    NetworkEvent { interface: String, rx_bytes_per_sec: u64, tx_bytes_per_sec: u64 },
    DeviceEvent { category: String, devices: Vec<String> },
    SensorEvent { name: String, value: f64 },
    UsbEvent { devices: Vec<String> },
    PciEvent { devices: Vec<String> },
    BluetoothEvent { devices: Vec<String> },
}

pub struct Dispatcher {
    sender: mpsc::UnboundedSender<TelemetryEvent>,
}

impl Dispatcher {
    pub fn new() -> (Self, mpsc::UnboundedReceiver<TelemetryEvent>) {
        let (sender, receiver) = mpsc::unbounded_channel();
        (Self { sender }, receiver)
    }

    pub fn send(&self, event: TelemetryEvent) {
        let _ = self.sender.send(event);
    }
}

pub struct LoggerSubscriber {
    receiver: mpsc::UnboundedReceiver<TelemetryEvent>,
}

impl LoggerSubscriber {
    pub fn new(receiver: mpsc::UnboundedReceiver<TelemetryEvent>) -> Self {
        Self { receiver }
    }

    pub async fn run(mut self) {
        while let Some(event) = self.receiver.recv().await {
            match event {
                TelemetryEvent::CpuEvent { usage } => {
                    log::info!("CPU: usage {:.2}%", usage);
                }
                TelemetryEvent::MemoryEvent { total, used } => {
                    log::info!("RAM: total {} KB, used {} KB", total, used);
                }
                TelemetryEvent::DiskEvent { name, total, available } => {
                    log::info!("Disk {}: total {} B, available {} B", name, total, available);
                }
                TelemetryEvent::NetworkEvent { interface, rx_bytes_per_sec, tx_bytes_per_sec } => {
                    log::info!("Network {}: RX {} B/s, TX {} B/s", interface, rx_bytes_per_sec, tx_bytes_per_sec);
                }
                TelemetryEvent::DeviceEvent { category, devices } => {
                    log::info!("{} devices: {:?}", category, devices);
                }
                TelemetryEvent::SensorEvent { name, value } => {
                    log::info!("Sensor {}: {:.2}", name, value);
                }
                TelemetryEvent::UsbEvent { devices } => {
                    log::info!("USB devices: {:?}", devices);
                }
                TelemetryEvent::PciEvent { devices } => {
                    log::info!("PCI devices: {:?}", devices);
                }
                TelemetryEvent::BluetoothEvent { devices } => {
                    log::info!("Bluetooth devices: {:?}", devices);
                }
            }
        }
    }
}