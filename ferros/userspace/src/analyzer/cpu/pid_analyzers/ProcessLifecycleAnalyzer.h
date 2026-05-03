#include "analyzer/cpu/pid_analyzers/ProcessLifecycleAnalyzer.h"

#include <iostream>

void ProcessLifecycleAnalyzer::analyze(const TelemetryBundle& bundle)
{
    for (const auto& e : bundle.cpu().getAllEvents())
    {
        updateState(e);
    }

    // Generate insights
    for (const auto& [pid, s] : state)
    {
        if (s.first_seen == std::numeric_limits<u64>::max())
            continue;

        u64 lifetime_ns = s.last_seen - s.first_seen;

        std::cout << "[Lifecycle] PID: " << pid
                  << " lifetime_ns: " << lifetime_ns
                  << " events: " << s.event_count;

        if (s.exited)
        {
            std::cout << " exit_code: " << s.exit_code;
        }

        std::cout << std::endl;
    }
}

void ProcessLifecycleAnalyzer::updateState(const cpu_event& e)
{
    auto& s = state[e.pid];

    if (s.first_seen == std::numeric_limits<u64>::max())
    {
        s.first_seen = e.timestamp_ns;
    }

    s.last_seen = e.timestamp_ns;
    s.event_count++;

    if (e.exit_code != 0)
    {
        s.exit_code = e.exit_code;
        s.exited = true;
    }
}

std::vector<ProcessLifecycleInsight> ProcessLifecycleAnalyzer::getInsights() const
{
    std::vector<ProcessLifecycleInsight> out;
    out.reserve(state.size());

    for (const auto& [pid, s] : state)
    {
        out.push_back({
            pid,
            s.first_seen,
            s.last_seen,
            s.event_count,
            s.exit_code,
            s.exited
        });
    }

    return out;
}