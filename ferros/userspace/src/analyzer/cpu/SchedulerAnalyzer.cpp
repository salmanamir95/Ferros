#include "analyzer/cpu/SchedulerAnalyzer.h"
#include <algorithm>

void SchedulerAnalyzer::analyze(const TelemetryBundle& bundle) {
    const auto& events = bundle.raw();
    
    for (const auto& ev : events) {
        if (ev.h.type == EVENT_SCHED_SWITCH) {
            handleSwitch(ev);
        } else if (ev.h.type == EVENT_PROCESS_FORK) {
            handleFork(ev);
        }
        total_processed++;
    }
}

void SchedulerAnalyzer::handleSwitch(const foc_event& ev) {
    const auto& s = ev.p.sw;
    u32 cpu = ev.h.cpu;
    u64 ts = ev.h.ts;
    u32 next_pid = s.next_pid;
    u32 prev_pid = s.prev_pid;
    
    auto& cpu_state = cpu_states[cpu];
    
    // 1. Userspace intelligence: Slice Accounting
    // We reconstruct the duration from the previous event on this CPU
    if (cpu_state.last_ts > 0 && cpu_state.current_pid == prev_pid) {
        u64 duration = ts - cpu_state.last_ts;
        auto& p_prev = process_map[prev_pid];
        p_prev.pid = prev_pid;
        p_prev.comm = s.prev_comm;
        p_prev.total_runtime_ns += duration;
        p_prev.slice_count++;
        
        // Causality Reconstruction
        // prev_state == 0 (TASK_RUNNING) usually means preemption in this context
        if (s.prev_state == 0) {
            p_prev.preemption_count++;
            p_prev.timeline.push_back({ts, cpu, "PREEMPT", "Preempted by " + std::to_string(next_pid)});
        } else {
            p_prev.yield_count++;
            p_prev.timeline.push_back({ts, cpu, "YIELD", "Voluntary yield (state=" + std::to_string(s.prev_state) + ")"});
        }
    }

    // 2. Intelligence: Migration Detection
    auto& p_next = process_map[next_pid];
    p_next.pid = next_pid;
    p_next.comm = s.next_comm;
    
    std::string detail = "Scheduled on CPU " + std::to_string(cpu);
    if (s.prev_cpu != cpu && s.prev_cpu != 0xFFFFFFFF) {
        detail += " (MIGRATED from CPU " + std::to_string(s.prev_cpu) + ")";
    }
    p_next.timeline.push_back({ts, cpu, "RUN", detail});
    
    // Update CPU state for next switch
    cpu_state.current_pid = next_pid;
    cpu_state.last_ts = ts;

    detectAnomalies(next_pid, p_next);
}

void SchedulerAnalyzer::handleFork(const foc_event& ev) {
    const auto& f = ev.p.fk;
    u32 parent = f.parent_pid;
    u32 child = f.child_pid;
    u64 ts = ev.h.ts;

    tree[parent].push_back(child);
    
    if (roots.find(parent) == roots.end() && process_map.find(parent) == process_map.end()) {
        roots.insert(parent);
    }
    roots.erase(child);

    auto& p_child = process_map[child];
    p_child.pid = child;
    p_child.comm = f.child_comm;
    p_child.timeline.push_back({ts, ev.h.cpu, "FORK", "Forked from " + std::to_string(parent)});
}

void SchedulerAnalyzer::detectAnomalies(u32 pid, const ProcessInfo& info) {
    if (tree.count(pid) && tree[pid].size() > 50) {
        std::string msg = "Possible Fork Bomb: PID " + std::to_string(pid) + " (" + info.comm + ") has " + std::to_string(tree[pid].size()) + " children";
        if (std::find(anomalies.begin(), anomalies.end(), msg) == anomalies.end()) {
            anomalies.push_back(msg);
        }
    }
}

void SchedulerAnalyzer::collectInsights(std::vector<std::shared_ptr<IInsight>>& insights) {
    auto insight = std::make_shared<SchedulerInsight>();
    insight->total_events = total_processed;
    
    for (const auto& [pid, info] : process_map) insight->processes[pid] = info;
    for (const auto& [parent, children] : tree) insight->parent_to_children[parent] = children;
    
    insight->tree_roots = roots;
    insight->anomalies = anomalies;
    
    // Placeholder Load Metric
    for (const auto& [cpu, state] : cpu_states) {
        insight->cpu_load[cpu] = 100.0; 
    }

    insights.push_back(insight);
}
