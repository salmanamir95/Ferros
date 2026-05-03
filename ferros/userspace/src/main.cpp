#include <iostream>
#include "loader.h"
#include "mods/CPUTelemetry.h"

int main()
{
    CPUTelemetry _cpu;
    int ret = start_ebpf(_cpu);

    std::cerr << "Program exited with code: " << ret << std::endl;
    return ret;
}