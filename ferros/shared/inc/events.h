#ifndef EVENTS_H
#define EVENTS_H

#include "types.h"

#define RINGBUF_SIZE (256 * 1024)

typedef enum {
    EVENT_SCHED_SWITCH = 1,
    EVENT_PROCESS_FORK = 2,
    EVENT_PROCESS_EXIT = 3
} foc_type_t;

/**
 * @brief Aligned header for identity stability
 * Size: 24 bytes (8-aligned)
 */
struct foc_header {
    u64 ts;
    u32 pid;
    u32 tgid;
    u32 cpu;
    u16 type;
    u16 reserved; 
} __attribute__((packed));

/**
 * @brief Lossless Scheduler Transition Payload
 */
struct foc_switch_payload {
    u32 prev_pid;
    u32 next_pid;

    u32 prev_cpu;     
    u32 next_cpu;

    s32 prev_prio;
    s32 next_prio;

    s64 prev_state;

    char prev_comm[16];
    char next_comm[16];
} __attribute__((packed));

/**
 * @brief Precise Process Relationship Payload
 */
struct foc_fork_payload {
    u32 parent_pid;
    u32 child_pid;

    u32 parent_tgid;
    u32 child_tgid;

    char parent_comm[16];
    char child_comm[16];
} __attribute__((packed));

/**
 * @brief Production-Grade Event Wrapper
 */
struct foc_event {
    struct foc_header h;

    union {
        struct foc_switch_payload sw;
        struct foc_fork_payload fk;
    } p;
} __attribute__((packed, aligned(8)));

#endif