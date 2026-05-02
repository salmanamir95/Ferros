use aya::maps::perf::PerfEventArray;
use aya::maps::MapData;
use aya::util::online_cpus;
use bytes::BytesMut;
use exec_monitor_common::ExecEvent;
use std::sync::mpsc;
use std::thread;
use std::time::Duration;

/// Encapsulates the multi-CPU event polling logic
pub struct EventPoller {
    perf_array: PerfEventArray<MapData>,
}

impl EventPoller {
    pub fn new(perf_array: PerfEventArray<MapData>) -> Self {
        Self { perf_array }
    }

    pub fn start(mut self, tx: mpsc::Sender<ExecEvent>) -> anyhow::Result<()> {
        for cpu_id in online_cpus().map_err(|(_, error)| error)? {
            let mut buf = self.perf_array.open(cpu_id, None)?;
            let tx_clone = tx.clone();

            thread::spawn(move || {
                let mut buffers = vec![BytesMut::with_capacity(1024); 10];
                loop {
                    if let Ok(events) = buf.read_events(&mut buffers) {
                        for i in 0..events.read {
                            let ptr = buffers[i].as_ptr() as *const ExecEvent;
                            let data = unsafe { ptr.read_unaligned() };
                            let _ = tx_clone.send(data);
                        }
                        if events.read == 0 {
                            thread::sleep(Duration::from_millis(50));
                        }
                    } else {
                        thread::sleep(Duration::from_millis(50));
                    }
                }
            });
        }
        Ok(())
    }
}
