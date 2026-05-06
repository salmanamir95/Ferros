#include "analyzer/cpu/pid_analyzers/ProcessLifecycleAnalyzer.h"

#include <cstring>
#include <string>

void ProcessLifecycleAnalyzer::analyze(const TelemetryBundle& bundle)
{
    const auto& events = bundle.raw();

    for (const auto& ev : events)
    {
        if (ev.h.type == EVENT_SCHED_SWITCH) {
            updateFromSwitch(ev);
        } else if (ev.h.type == EVENT_PROCESS_FORK) {
            updateFromFork(ev);
        }
    }
}

void ProcessLifecycleAnalyzer::collectInsights(std::vector<std::shared_ptr<IInsight>>& insights)
{
    auto currentInsights = getInsights();
    for (const auto& insight : currentInsights)
    {
        insights.push_back(std::make_shared<ProcessLifecycleInsight>(insight));
    }
}

void ProcessLifecycleAnalyzer::updateFromSwitch(const foc_event& ev)
{
    const auto& s = ev.p.sw;
    // Process arriving on CPU
    auto& next = state[s.next_pid];
    if (next.first_seen == std::numeric_limits<u64>::max()) {
        next.first_seen = ev.h.ts;
        std::strncpy(next.comm.data(), s.next_comm, 16);
    }
    next.last_seen = ev.h.ts;
    next.event_count++;

    // Process leaving CPU
    auto& prev = state[s.prev_pid];
    prev.last_seen = ev.h.ts;
    prev.event_count++;
}

void ProcessLifecycleAnalyzer::updateFromFork(const foc_event& ev)
{
    const auto& f = ev.p.fk;
    auto& child = state[f.child_pid];
    child.first_seen = ev.h.ts;
    child.last_seen = ev.h.ts;
    child.event_count = 1;
    std::strncpy(child.comm.data(), f.child_comm, 16);

    auto& parent = state[f.parent_pid];
    std::strncpy(parent.comm.data(), f.parent_comm, 16);
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