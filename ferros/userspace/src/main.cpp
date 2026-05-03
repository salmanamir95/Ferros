#include <iostream>
#include "loader.h"

int main()
{
    int ret = start_ebpf();

    std::cerr << "Program exited with code: " << ret << std::endl;
    return ret;
}