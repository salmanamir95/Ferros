#pragma once

#include <cstdint>

enum class TelemetryType : uint8_t
{
    CPU = 0,
    MEMORY = 1,
    SCHEDULER = 2,
    NETWORK = 3,
    DISK = 4
};