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

        // Attach sys_enter_execve
        let program: &mut TracePoint = ebpf.program_mut("sys_enter_execve").unwrap().try_into()?;
        program.load()?;
        program.attach("syscalls", "sys_enter_execve")?;

        // Attach sys_enter_connect
        let program: &mut TracePoint = ebpf.program_mut("sys_enter_connect").unwrap().try_into()?;
        program.load()?;
        program.attach("syscalls", "sys_enter_connect")?;

        // Attach sys_enter_openat
        let program: &mut TracePoint = ebpf.program_mut("sys_enter_openat").unwrap().try_into()?;
        program.load()?;
        program.attach("syscalls", "sys_enter_openat")?;

        // Attach sys_enter_clone
        let program: &mut TracePoint = ebpf.program_mut("sys_enter_clone").unwrap().try_into()?;
        program.load()?;
        program.attach("syscalls", "sys_enter_clone")?;

        // Attach sys_enter_write
        let program: &mut TracePoint = ebpf.program_mut("sys_enter_write").unwrap().try_into()?;
        program.load()?;
        program.attach("syscalls", "sys_enter_write")?;

        Ok(Self { ebpf })
    }
}
