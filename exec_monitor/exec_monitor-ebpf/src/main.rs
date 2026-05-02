#![no_std]
#![no_main]

use aya_ebpf::{
    helpers::{
        bpf_get_current_comm, bpf_get_current_pid_tgid, bpf_get_current_task,
        bpf_get_current_uid_gid, bpf_ktime_get_ns, bpf_probe_read_kernel,
    },
    macros::{map, tracepoint},
    maps::PerfEventArray,
    programs::TracePointContext,
};
use exec_monitor_common::SyscallEvent;

#[map]
pub static EVENTS: PerfEventArray<SyscallEvent> = PerfEventArray::new(0);

// Helper to get PPID with safe fallback if BTF isn't generated
#[inline(always)]
fn get_ppid() -> u32 {
    // For MVP, if task_struct bindings aren't fully generated, we fallback to 0.
    // In a fully integrated BTF build, we would use:
    // let task = unsafe { bpf_get_current_task_btf() as *const task_struct };
    // let ppid = unsafe { (*(*task).real_parent).tgid };
    
    // As a safe fallback without vmlinux bindings:
    0
}

#[inline(always)]
fn create_and_emit_event(ctx: &TracePointContext) -> Result<u32, u32> {
    let pid_tgid = bpf_get_current_pid_tgid();
    let pid = (pid_tgid >> 32) as u32;
    let uid_gid = bpf_get_current_uid_gid();
    let uid = (uid_gid & 0xFFFFFFFF) as u32;
    let gid = (uid_gid >> 32) as u32;

    let comm = bpf_get_current_comm().unwrap_or([0; 16]);
    let boot_time_ns = unsafe { bpf_ktime_get_ns() };
    let ppid = get_ppid();

    let event = SyscallEvent {
        boot_time_ns,
        pid,
        ppid,
        uid,
        gid,
        syscall_id: 0, // Optional metadata, not strictly relied upon for logic
        comm,
        cpu_usage_snapshot: 0,
        memory_rss_snapshot: 0,
        network_tx: 0,
        network_rx: 0,
        temperature: 0,
        power_mw: 0,
        threat_score: 0,
    };

    EVENTS.output(ctx, &event, 0);
    Ok(0)
}

#[tracepoint(category = "syscalls", name = "sys_enter_execve")]
pub fn sys_enter_execve(ctx: TracePointContext) -> u32 {
    create_and_emit_event(&ctx).unwrap_or(0)
}

#[tracepoint(category = "syscalls", name = "sys_enter_connect")]
pub fn sys_enter_connect(ctx: TracePointContext) -> u32 {
    create_and_emit_event(&ctx).unwrap_or(0)
}

#[tracepoint(category = "syscalls", name = "sys_enter_openat")]
pub fn sys_enter_openat(ctx: TracePointContext) -> u32 {
    create_and_emit_event(&ctx).unwrap_or(0)
}

#[tracepoint(category = "syscalls", name = "sys_enter_clone")]
pub fn sys_enter_clone(ctx: TracePointContext) -> u32 {
    create_and_emit_event(&ctx).unwrap_or(0)
}

#[tracepoint(category = "syscalls", name = "sys_enter_write")]
pub fn sys_enter_write(ctx: TracePointContext) -> u32 {
    create_and_emit_event(&ctx).unwrap_or(0)
}

#[cfg(not(test))]
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

#[unsafe(link_section = "license")]
#[unsafe(no_mangle)]
static LICENSE: [u8; 13] = *b"Dual MIT/GPL\0";