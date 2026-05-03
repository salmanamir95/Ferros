file(GLOB_RECURSE USERSPACE_SOURCES

    # Main entry and top-level sources
    ${PROJECT_ROOT}/userspace/src/*.cpp
    ${PROJECT_ROOT}/userspace/src/*.c

    # Core system
    ${PROJECT_ROOT}/userspace/src/core/*.cpp
    ${PROJECT_ROOT}/userspace/src/core/*.c

    # Telemetry layer
    ${PROJECT_ROOT}/userspace/src/telemetry/*.cpp
    ${PROJECT_ROOT}/userspace/src/telemetry/*.c

    # Analyzer layer
    ${PROJECT_ROOT}/userspace/src/analyzer/*.cpp
    ${PROJECT_ROOT}/userspace/src/analyzer/*.c

    # Internal utilities (NOT shared replacement)
    ${PROJECT_ROOT}/userspace/src/common/*.cpp
    ${PROJECT_ROOT}/userspace/src/common/*.c
)