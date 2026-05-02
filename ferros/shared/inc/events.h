#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"
#include "constants.h"

struct cpu_event
{
    u32 pid;
    u64 runtime_ns;
    char comm[TASK_COMM_LEN];
};

#endif