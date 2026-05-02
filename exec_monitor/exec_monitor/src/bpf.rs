use aya::programs::RawTracePoint;
use aya::{include_bytes_aligned, Ebpf};
use aya::maps::HashMap;

pub struct BpfApp {
    pub ebpf: Ebpf,
}

impl BpfApp {
    pub fn load() -> anyhow::Result<Self> {
        let rlim = libc::rlimit {
            rlim_cur: libc::RLIM_INFINITY,
            rlim_max: libc::RLIM_INFINITY,
        };
        unsafe {
            libc::setrlimit(libc::RLIMIT_MEMLOCK, &rlim);
        }

        let mut ebpf = Ebpf::load(include_bytes_aligned!(concat!(
            env!("OUT_DIR"),
            "/exec_monitor"
        )))?;

        let program: &mut RawTracePoint = ebpf.program_mut("sys_enter_monitor").unwrap().try_into()?;
        program.load()?;
        program.attach("sys_enter")?;

        Ok(Self { ebpf })
    }

    pub fn init_syscalls(&mut self, syscall_ids: &[u32]) -> anyhow::Result<()> {
        let mut map: HashMap<_, u32, u8> = HashMap::try_from(self.ebpf.map_mut("MONITORED_SYSCALLS").unwrap())?;
        for &id in syscall_ids {
            map.insert(id, 1, 0)?;
        }
        Ok(())
    }
}
