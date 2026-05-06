#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_tracing.h>
#include "events.h"

char LICENSE[] SEC("license") = "GPL";

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, RINGBUF_SIZE);
} events SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, u32);
    __type(value, u32);
} pid_last_cpu SEC(".maps");

SEC("tracepoint/sched/sched_switch")
int handle_sched_switch(struct trace_event_raw_sched_switch *ctx)
{
    struct foc_event *e;
    u64 now = bpf_ktime_get_ns();
    u32 cpu = bpf_get_smp_processor_id();
    u32 next_pid = ctx->next_pid; // Copy to stack for map lookup safety

    e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
    if (!e) return 0;

    // 1. Header (Aligned)
    e->h.ts = now;
    e->h.pid = next_pid;
    e->h.tgid = (u32)bpf_get_current_pid_tgid(); 
    e->h.cpu = cpu;
    e->h.type = EVENT_SCHED_SWITCH;
    e->h.reserved = 0;

    // 2. Payload
    e->p.sw.prev_pid = ctx->prev_pid;
    e->p.sw.next_pid = next_pid;
    e->p.sw.next_cpu = cpu;
    
    // Map Lookup using stack variable
    u32 *last_cpu = bpf_map_lookup_elem(&pid_last_cpu, &next_pid);
    if (last_cpu) {
        e->p.sw.prev_cpu = *last_cpu;
    } else {
        e->p.sw.prev_cpu = cpu;
    }
    bpf_map_update_elem(&pid_last_cpu, &next_pid, &cpu, BPF_ANY);

    e->p.sw.prev_prio = ctx->prev_prio;
    e->p.sw.next_prio = ctx->next_prio;
    e->p.sw.prev_state = ctx->prev_state;

    bpf_probe_read_kernel_str(&e->p.sw.prev_comm, sizeof(e->p.sw.prev_comm), ctx->prev_comm);
    bpf_probe_read_kernel_str(&e->p.sw.next_comm, sizeof(e->p.sw.next_comm), ctx->next_comm);

    bpf_ringbuf_submit(e, 0);
    return 0;
}

SEC("tracepoint/sched/sched_process_fork")
int handle_process_fork(struct trace_event_raw_sched_process_fork *ctx)
{
    struct foc_event *e;
    u64 now = bpf_ktime_get_ns();
    u64 pid_tgid = bpf_get_current_pid_tgid();

    e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
    if (!e) return 0;

    e->h.ts = now;
    e->h.pid = (u32)(pid_tgid >> 32);
    e->h.tgid = (u32)pid_tgid;
    e->h.cpu = bpf_get_smp_processor_id();
    e->h.type = EVENT_PROCESS_FORK;
    e->h.reserved = 0;

    e->p.fk.parent_pid = ctx->parent_pid;
    e->p.fk.child_pid = ctx->child_pid;
    
    struct task_struct *task = (struct task_struct *)bpf_get_current_task();
    e->p.fk.parent_tgid = BPF_CORE_READ(task, tgid);
    e->p.fk.child_tgid = e->p.fk.parent_tgid; 

    bpf_get_current_comm(&e->p.fk.parent_comm, sizeof(e->p.fk.parent_comm));
    bpf_probe_read_kernel_str(&e->p.fk.child_comm, sizeof(e->p.fk.child_comm), e->p.fk.parent_comm);

    bpf_ringbuf_submit(e, 0);
    return 0;
}