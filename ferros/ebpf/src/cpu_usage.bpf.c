#include "vmlinux.h"

#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

#include "events.h"
#include "constants.h"
char LICENSE[] SEC("license") = "GPL";

/* =========================================================
   Stores timestamp when process was scheduled IN
========================================================= */
struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_PID_ENTRIES);

    __type(key, __u32);
    __type(value, __u64);

} start SEC(".maps");

/* =========================================================
   Stores total runtime per PID
========================================================= */
struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_PID_ENTRIES);

    __type(key, __u32);
    __type(value, __u64);

} runtime SEC(".maps");

/* =========================================================
   Ring buffer (kernel → userspace stream)
========================================================= */
struct
{
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, RINGBUF_SIZE);
} events SEC(".maps");

/* =========================================================
   Scheduler tracepoint
========================================================= */
SEC("tracepoint/sched/sched_switch")
int handle_sched_switch(struct trace_event_raw_sched_switch *ctx)
{
    __u64 ts = bpf_ktime_get_ns();

    __u32 prev_pid = ctx->prev_pid;
    __u32 next_pid = ctx->next_pid;

    if (prev_pid == 0)
        return 0;

    /* ---------------------------
       Handle OUT (prev task)
    --------------------------- */

    __u64 *start_time = bpf_map_lookup_elem(&start, &prev_pid);

    if (start_time)
    {
        __u64 delta = ts - *start_time;

        __u64 *total = bpf_map_lookup_elem(&runtime, &prev_pid);

        if (total)
            *total += delta;
        else
            bpf_map_update_elem(&runtime, &prev_pid, &delta, BPF_ANY);

        /* ---------------------------
           Push event to userspace
        --------------------------- */

        struct cpu_event *e;

        e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
        if (!e)
            return 0;

        e->pid = prev_pid;
        e->runtime_ns = delta;

        bpf_probe_read_kernel_str(
            e->comm,
            sizeof(e->comm),
            ctx->prev_comm);

        bpf_ringbuf_submit(e, 0);
    }

    /* ---------------------------
       Handle IN (next task)
    --------------------------- */

    bpf_map_update_elem(&start, &next_pid, &ts, BPF_ANY);

    return 0;
}