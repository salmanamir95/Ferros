#ifndef EVENTS_H
#define EVENTS_H

typedef unsigned int __u32;
typedef unsigned long long __u64;

struct cpu_event {
    __u32 pid;
    __u64 runtime_ns;
    char comm[16];
};

#endif