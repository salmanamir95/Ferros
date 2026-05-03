#pragma once

#include "events.h"


struct ProcessLifecycleInsight
{
    u32 pid;
    u64 first_seen;
    u64 last_seen;
    u64 event_count;
    u32 exit_code;
    bool exited;
};