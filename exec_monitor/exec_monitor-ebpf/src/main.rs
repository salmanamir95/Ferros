#![no_std]
#![no_main]

use aya_ebpf::{
    helpers::{bpf_get_current_comm, bpf_get_current_pid_tgid, bpf_get_current_uid_gid, bpf_ktime_get_ns},
    macros::{map, raw_tracepoint},
    maps::{HashMap, PerfEventArray},
    programs::RawTracePointContext,
};
use exec_monitor_common::SyscallEvent;

#[map]
pub static EVENTS: PerfEventArray<SyscallEvent> = PerfEventArray::new(0);

#[map]
pub static MONITORED_SYSCALLS: HashMap<u32, u8> = HashMap::with_max_entries(256, 0);

#[raw_tracepoint(tracepoint = "sys_enter")]
pub fn sys_enter_monitor(ctx: RawTracePointContext) -> u32 {
    match try_sys_enter_monitor(ctx) {
        Ok(ret) => ret,
        Err(ret) => ret,
    }
}

fn try_sys_enter_monitor(ctx: RawTracePointContext) -> Result<u32, u32> {
    // raw_tracepoint sys_enter args:
    // ctx.arg(0) is struct pt_regs *, ctx.arg(1) is the syscall ID
    let syscall_id: u32 = unsafe { ctx.arg::<u32>(1) };

    // Check if syscall is monitored
    if unsafe { MONITORED_SYSCALLS.get(&syscall_id) }.is_none() {
        return Ok(0);
    }

    let pid_tgid = bpf_get_current_pid_tgid();
    let pid = (pid_tgid >> 32) as u32; // tgid is pid
    let uid_gid = bpf_get_current_uid_gid();
    let uid = (uid_gid & 0xFFFFFFFF) as u32;
    let gid = (uid_gid >> 32) as u32;

    let comm = bpf_get_current_comm().unwrap_or([0; 16]);
    let timestamp = unsafe { bpf_ktime_get_ns() };

    let event = SyscallEvent {
        timestamp,
        pid,
        ppid: 0, // Need task_struct for this, keeping 0 for MVP or we can get it via bpf_get_current_task
        uid,
        gid,
        syscall_id,
        comm,
        cpu_usage: 0,
        memory_usage: 0,
        network_tx: 0,
        network_rx: 0,
        temperature: 0,
        power_mw: 0,
        threat_score: 0,
    };
    EVENTS.output(&ctx, &event, 0);

    Ok(0)
}

#[cfg(not(test))]
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

#[unsafe(link_section = "license")]
#[unsafe(no_mangle)]
static LICENSE: [u8; 13] = *b"Dual MIT/GPL\0";