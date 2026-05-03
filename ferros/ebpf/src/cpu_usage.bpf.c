#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

#include "events.h"

char LICENSE[] SEC("license") = "GPL";

/* Ring buffer */
struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, RINGBUF_SIZE);
} events SEC(".maps");

SEC("tracepoint/sched/sched_switch")
int handle_sched_switch(struct trace_event_raw_sched_switch *ctx)
{
    u64 ts = bpf_ktime_get_ns();

    struct cpu_event *e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
    if (!e)
        return 0;

    /* Fill event */
    e->pid = ctx->prev_pid;
    e->tgid = ctx->prev_pid;
    e->cpu = bpf_get_smp_processor_id();
    e->timestamp_ns = ts;
    e->runtime_ns = 0;
    e->exit_code = 0;

    bpf_probe_read_kernel_str(&e->comm, sizeof(e->comm), ctx->prev_comm);

    bpf_ringbuf_submit(e, 0);

    return 0;
}