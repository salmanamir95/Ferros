# Telemetry System

A modular, event-driven user-space telemetry monitoring application written in Rust for Ubuntu Linux. This system collects comprehensive system telemetry data and logs it through a centralized event bus.

## Architecture

- **Modular Design**: Separate modules for different telemetry sources (system, network, devices, sensors, USB, PCI, Bluetooth)
- **Event-Driven**: Uses a publisher-subscriber pattern with a central event dispatcher
- **Async**: Built with Tokio for non-blocking 1-second telemetry cycles
- **Production-Ready**: Proper error handling, no unwrap() panics

## Features

- **CPU Monitoring**: Average CPU usage percentage
- **RAM Monitoring**: Total and used memory
- **Hard Disk Monitoring**: Space usage for all mounted disks
- **Network Monitoring**: RX/TX bytes per second for all interfaces
- **Input Devices**: Lists input device paths and detailed info
- **Output Devices**: Audio and graphics device detection
- **Cameras**: V4L2 video device detection
- **Printers**: USB printer detection
- **Temperature Sensors**: Hardware monitor temperatures
- **Power Systems**: Battery capacity monitoring
- **USB Topology**: USB device enumeration
- **PCI Devices**: PCI device listing
- **Bluetooth Devices**: HCI device detection

## Requirements

- Rust 1.70 or later
- Ubuntu Linux (or compatible Linux distribution)
- Tokio async runtime

## Building

```bash
cargo build --release
```

## Running

```bash
./target/release/telemetry
```

The application runs continuously, collecting telemetry every 1 second and logging events to `telemetry.log`.

## Event System

- `TelemetryEvent` enum defines all event types
- `Dispatcher` sends events to subscribers
- `LoggerSubscriber` logs events to file
- Extensible for future exporters (Prometheus, Kafka, etc.)

## Dependencies

- `sysinfo`: System information
- `evdev`: Input device enumeration
- `tokio`: Async runtime and timing
- `log` and `simplelog`: Logging

## Notes

- The application runs in user space and does not require root privileges for basic monitoring.
- For full access to input devices, you may need to add the user to the `input` group.
- Network rates are calculated as bytes transferred since the last check (every 5 seconds).