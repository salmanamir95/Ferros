pub mod events;
pub mod system;
pub mod network;
pub mod devices;
pub mod sensors;
pub mod usb;
pub mod pci;
pub mod bluetooth;

use events::{Dispatcher, LoggerSubscriber};
use system::SystemCollector;
use network::NetworkCollector;
use devices::DeviceCollector;
use sensors::SensorCollector;
use usb::UsbCollector;
use pci::PciCollector;
use bluetooth::BluetoothCollector;
use log::{LevelFilter};
use simplelog::{Config, WriteLogger};
use std::fs::File;
use tokio::time::{interval, Duration};

pub async fn start_telemetry() -> Result<(), Box<dyn std::error::Error>> {
    // Logger initialization is now handled by the main daemon (exec_monitor)
    // to prevent "logger already initialized" errors.

    // Create event system
    let (dispatcher, receiver) = Dispatcher::new();
    let logger = LoggerSubscriber::new(receiver);
    tokio::spawn(async move {
        logger.run().await;
    });

    // Create collectors
    let mut system_collector = SystemCollector::new();
    let mut network_collector = NetworkCollector::new();
    let device_collector = DeviceCollector::new();
    let sensor_collector = SensorCollector::new();
    let usb_collector = UsbCollector::new();
    let pci_collector = PciCollector::new();
    let bluetooth_collector = BluetoothCollector::new();

    // 1-second interval
    let mut interval = interval(Duration::from_secs(1));

    loop {
        interval.tick().await;

        // Collect telemetry
        system_collector.collect(&dispatcher);
        network_collector.collect(&dispatcher);
        device_collector.collect(&dispatcher);
        sensor_collector.collect(&dispatcher);
        usb_collector.collect(&dispatcher);
        pci_collector.collect(&dispatcher);
        bluetooth_collector.collect(&dispatcher);
    }
}
