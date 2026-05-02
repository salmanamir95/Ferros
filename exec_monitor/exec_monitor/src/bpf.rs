use aya::programs::TracePoint;
use aya::{include_bytes_aligned, Ebpf};

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

        let program: &mut TracePoint = ebpf.program_mut("exec_monitor").unwrap().try_into()?;
        program.load()?;
        program.attach("sched", "sched_process_exec")?;

        Ok(Self { ebpf })
    }
}
