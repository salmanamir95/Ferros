#![no_std]
#![no_main]

use aya_ebpf::{
    helpers::{bpf_get_current_comm, bpf_get_current_pid_tgid},
    macros::{map, tracepoint},
    maps::PerfEventArray,
    programs::TracePointContext,
};
use exec_monitor_common::ExecEvent;

#[map]
pub static EVENTS: PerfEventArray<ExecEvent> = PerfEventArray::new(0);

#[tracepoint(category = "sched", name = "sched_process_exec")]
pub fn exec_monitor(ctx: TracePointContext) -> u32 {
    match try_exec_monitor(ctx) {
        Ok(ret) => ret,
        Err(ret) => ret,
    }
}

fn try_exec_monitor(ctx: TracePointContext) -> Result<u32, u32> {
    let pid = bpf_get_current_pid_tgid() as u32;
    let comm = bpf_get_current_comm().unwrap_or([0; 16]);
    
    let event = ExecEvent { pid, comm };
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