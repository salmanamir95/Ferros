#include "analyzer/cpu/pid_analyzers/ProcessLifecycleAnalyzer.h"

#include <cstring>
#include <string>

void ProcessLifecycleAnalyzer::analyze(
    const TelemetryBundle& bundle)
{
    const auto& events =
        bundle.cpu().getAllEvents();

    for (size_t i = last_processed;
         i < events.size();
         ++i)
    {
        updateState(events[i]);
    }

    last_processed = events.size();
}

void ProcessLifecycleAnalyzer::updateState(
    const cpu_event& e)
{
    auto& s = state[e.pid];

    // First time seeing this PID
    if (s.first_seen ==
        std::numeric_limits<u64>::max())
    {
        s.first_seen = e.timestamp_ns;

        // Store process name once
        std::memcpy(
            s.comm.data(),
            e.comm,
            sizeof(e.comm));
    }

    s.last_seen = e.timestamp_ns;
    s.event_count++;

    // NOTE:
    // Ideally this should be based on an explicit
    // PROCESS_EXIT event flag/type instead of
    // exit_code != 0 because exit_code 0 is valid.
    if (e.exit_code != 0)
    {
        s.exit_code = e.exit_code;
        s.exited = true;
    }
}

std::vector<ProcessLifecycleInsight>
ProcessLifecycleAnalyzer::getInsights() const
{
    std::vector<ProcessLifecycleInsight> out;

    out.reserve(state.size());

    for (const auto& [pid, s] : state)
    {
        out.push_back({
            pid,

            // Safe conversion from fixed char buffer
            // to std::string
            std::string(
                s.comm.data(),
                strnlen(
                    s.comm.data(),
                    s.comm.size())),

            s.first_seen,
            s.last_seen,
            s.event_count,
            s.exit_code,
            s.exited
        });
    }

    return out;
}