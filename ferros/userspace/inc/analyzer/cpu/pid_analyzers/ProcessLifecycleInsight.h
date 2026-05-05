#pragma once

#include <string>
#include <cstdint>
#include "types.h"
struct ProcessLifecycleInsight
{
    u32 pid;
    std::string comm;

    u64 first_seen;
    u64 last_seen;
    u64 event_count;

    u32 exit_code;
    bool exited;

    // 👇 ADD THIS
    ProcessLifecycleInsight(
        u32 pid_,
        std::string comm_,
        u64 first_seen_,
        u64 last_seen_,
        u64 event_count_,
        u32 exit_code_,
        bool exited_)
        : pid(pid_),
          comm(std::move(comm_)),
          first_seen(first_seen_),
          last_seen(last_seen_),
          event_count(event_count_),
          exit_code(exit_code_),
          exited(exited_)
    {}
};