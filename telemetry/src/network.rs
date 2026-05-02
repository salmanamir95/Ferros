use sysinfo::Networks;
use std::collections::HashMap;
use crate::events::Dispatcher;

pub struct NetworkCollector {
    networks: Networks,
    prev_received: HashMap<String, u64>,
    prev_transmitted: HashMap<String, u64>,
}

impl NetworkCollector {
    pub fn new() -> Self {
        let networks = Networks::new_with_refreshed_list();
        let mut prev_received = HashMap::new();
        let mut prev_transmitted = HashMap::new();
        for (interface_name, data) in networks.iter() {
            prev_received.insert(interface_name.clone(), data.received());
            prev_transmitted.insert(interface_name.clone(), data.transmitted());
        }
        Self {
            networks,
            prev_received,
            prev_transmitted,
        }
    }

    pub fn collect(&mut self, dispatcher: &Dispatcher) {
        self.networks.refresh();
        for (interface_name, data) in self.networks.iter() {
            let received = data.received();
            let transmitted = data.transmitted();
            let prev_r = self.prev_received.get(interface_name).copied().unwrap_or(0);
            let prev_t = self.prev_transmitted.get(interface_name).copied().unwrap_or(0);
            let rx_bytes_per_sec = received.saturating_sub(prev_r);
            let tx_bytes_per_sec = transmitted.saturating_sub(prev_t);
            dispatcher.send(crate::events::TelemetryEvent::NetworkEvent {
                interface: interface_name.clone(),
                rx_bytes_per_sec,
                tx_bytes_per_sec,
            });
            self.prev_received.insert(interface_name.clone(), received);
            self.prev_transmitted.insert(interface_name.clone(), transmitted);
        }
    }
}