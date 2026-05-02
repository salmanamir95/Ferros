#![no_std]

#[repr(C)]
#[derive(Clone, Copy)]
pub struct SyscallEvent {
    pub timestamp: u64,

    pub pid: u32,
    pub ppid: u32,

    pub uid: u32,
    pub gid: u32,

    pub syscall_id: u32,

    pub comm: [u8; 16],

    // Hardware telemetry populated by userspace
    pub cpu_usage: u32,
    pub memory_usage: u64,

    pub network_tx: u64,
    pub network_rx: u64,

    pub temperature: i32,
    pub power_mw: u32,

    pub threat_score: u8,
}
