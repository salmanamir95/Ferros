#pragma once
#include <vector>
#include "events.h"

class CPUTelemetry
{
public:
    CPUTelemetry() = default;
    ~CPUTelemetry() = default;

    // --------------------
    // CREATE
    // --------------------
    void addEvent(const cpu_event &ev);

    // --------------------
    // READ (basic)
    // --------------------
    const std::vector<cpu_event> &getAllEvents() const;

    std::vector<cpu_event> getByPID(u32 pid) const;
    std::vector<cpu_event> getByTGID(u32 tgid) const;
    std::vector<cpu_event> getByCPU(u32 cpu) const;

    // --------------------
    // TIME-BASED QUERY (IMPORTANT)
    // --------------------
    std::vector<cpu_event> getByTimestampRange(
        u64 start_ns,
        u64 end_ns) const;

    // --------------------
    // RUNTIME FILTER (NEW)
    // --------------------
    std::vector<cpu_event> getByRuntimeGreaterThan(
        u64 runtime_ns) const;

    std::vector<cpu_event> getByRuntimeLessThan(
        u64 runtime_ns) const;

    std::vector<cpu_event> getByRuntimeRange(
        u64 min_runtime_ns,
        u64 max_runtime_ns) const;


    // --------------------
    // UPDATE (optional)
    // --------------------
    bool updateEventTimestamp(size_t index, u64 new_ts);

    // --------------------
    // DELETE (optional)
    // --------------------
    void removeEvent(size_t index);
    void clear();

private:
    std::vector<cpu_event> events;
};