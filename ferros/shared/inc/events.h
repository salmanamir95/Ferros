#ifndef EVENTS_H
#define EVENTS_H

#define RINGBUF_SIZE (256 * 1024)

struct cpu_event {
    unsigned int pid;
    unsigned int tgid;
    unsigned int cpu;
    unsigned int exit_code;

    unsigned long long timestamp_ns;
    unsigned long long runtime_ns;

    char comm[16];
};

#endif