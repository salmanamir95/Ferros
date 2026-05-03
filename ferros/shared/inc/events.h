#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

#define RINGBUF_SIZE (256 * 1024)

struct cpu_event {
    u32 pid;
    u32 tgid;
    u32 cpu;
    u32 exit_code;

    u64 timestamp_ns;
    u64 runtime_ns;

    char comm[16];
};

#endif