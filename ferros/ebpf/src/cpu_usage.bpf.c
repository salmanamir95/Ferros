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

SEC("tracepoint/sched/sched_switch")
int handle_sched_switch(struct trace_event_raw_sched_switch *ctx)
{
    struct cpu_event *e;

    /* Reserve space in the ring buffer */
    e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
    if (!e) {
        bpf_printk("FERROS: ringbuf reserve failed");
        return 0;
    }

    /* Fill event metadata from the tracepoint context */
    e->pid = ctx->next_pid;
    e->tgid = 0; // Not directly available in the raw tracepoint struct
    e->cpu = bpf_get_smp_processor_id();
    e->timestamp_ns = bpf_ktime_get_ns();
    
    /* Copy process name from the 'next' task */
    bpf_probe_read_kernel_str(&e->comm, sizeof(e->comm), ctx->next_comm);

    /* Submit the event to userspace */
    bpf_ringbuf_submit(e, 0);

    return 0;
}