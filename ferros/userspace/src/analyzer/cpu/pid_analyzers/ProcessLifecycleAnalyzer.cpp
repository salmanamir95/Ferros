#include "analyzer/cpu/pid_analyzers/ProcessLifecycleAnalyzer.h"

#include <cstring>
#include <string>

void ProcessLifecycleAnalyzer::analyze(const TelemetryBundle& bundle)
{
    const auto& events = bundle.cpu().getAllEvents();

    for (size_t i = last_processed; i < events.size(); ++i)
    {
        updateState(events[i]);
    }

    last_processed = events.size();
}

void ProcessLifecycleAnalyzer::collectInsights(std::vector<std::shared_ptr<IInsight>>& insights)
{
    auto currentInsights = getInsights();
    for (const auto& insight : currentInsights)
    {
        insights.push_back(std::make_shared<ProcessLifecycleInsight>(insight));
    }
}

void ProcessLifecycleAnalyzer::updateState(const cpu_event& e)
{
    auto& s = state[e.pid];

    if (s.first_seen == std::numeric_limits<u64>::max())
    {
        s.first_seen = e.timestamp_ns;
        std::memcpy(s.comm.data(), e.comm, sizeof(e.comm));
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
            std::string(s.comm.data(), strnlen(s.comm.data(), s.comm.size())),
            s.first_seen,
            s.last_seen,
            s.event_count,
            s.exit_code,
            s.exited
        });
    }

    return out;
}