#include "CPUTelemetry.h"
#include <algorithm>

// --------------------
// CREATE
// --------------------
void CPUTelemetry::addEvent(const cpu_event &ev)
{
    events.push_back(ev);
}

// --------------------
// READ (ALL)
// --------------------
const std::vector<cpu_event> &CPUTelemetry::getAllEvents() const
{
    return events;
}

// --------------------
// FILTER BY PID
// --------------------
std::vector<cpu_event> CPUTelemetry::getByPID(u32 pid) const
{
    std::vector<cpu_event> result;

    for (const auto &e : events)
        if (e.pid == pid)
            result.push_back(e);

    return result;
}

// --------------------
// FILTER BY TGID
// --------------------
std::vector<cpu_event> CPUTelemetry::getByTGID(u32 tgid) const
{
    std::vector<cpu_event> result;

    for (const auto &e : events)
        if (e.tgid == tgid)
            result.push_back(e);

    return result;
}

// --------------------
// FILTER BY CPU
// --------------------
std::vector<cpu_event> CPUTelemetry::getByCPU(u32 cpu) const
{
    std::vector<cpu_event> result;

    for (const auto &e : events)
        if (e.cpu == cpu)
            result.push_back(e);

    return result;
}

// --------------------
// TIME RANGE QUERY (CORE)
// --------------------
std::vector<cpu_event> CPUTelemetry::getByTimestampRange(
    u64 start_ns,
    u64 end_ns) const
{
    std::vector<cpu_event> result;

    for (const auto &e : events)
    {
        if (e.timestamp_ns >= start_ns &&
            e.timestamp_ns <= end_ns)
        {
            result.push_back(e);
        }
    }

    return result;
}

// --------------------
// RUNTIME FILTERS
// --------------------
std::vector<cpu_event> CPUTelemetry::getByRuntimeGreaterThan(u64 runtime_ns) const
{
    std::vector<cpu_event> result;

    for (const auto &e : events)
        if (e.runtime_ns > runtime_ns)
            result.push_back(e);

    return result;
}

std::vector<cpu_event> CPUTelemetry::getByRuntimeLessThan(u64 runtime_ns) const
{
    std::vector<cpu_event> result;

    for (const auto &e : events)
        if (e.runtime_ns < runtime_ns)
            result.push_back(e);

    return result;
}

std::vector<cpu_event> CPUTelemetry::getByRuntimeRange(
    u64 min_runtime_ns,
    u64 max_runtime_ns) const
{
    std::vector<cpu_event> result;

    for (const auto &e : events)
    {
        if (e.runtime_ns >= min_runtime_ns &&
            e.runtime_ns <= max_runtime_ns)
        {
            result.push_back(e);
        }
    }

    return result;
}

// --------------------
// UPDATE (RARE - NOT IDEAL IN TELEMETRY)
// --------------------
bool CPUTelemetry::updateEventTimestamp(size_t index, u64 new_ts)
{
    if (index >= events.size())
        return false;

    events[index].timestamp_ns = new_ts;
    return true;
}

// --------------------
// DELETE
// --------------------
void CPUTelemetry::removeEvent(size_t index)
{
    if (index >= events.size())
        return;

    events.erase(events.begin() + index);
}

// --------------------
// CLEAR
// --------------------
void CPUTelemetry::clear()
{
    events.clear();
}