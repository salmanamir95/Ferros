#ifndef LOADER_H
#define LOADER_H

/*
    Starts eBPF program lifecycle:
    - load object
    - attach tracepoint
    - read events
*/
int start_ebpf();

#endif