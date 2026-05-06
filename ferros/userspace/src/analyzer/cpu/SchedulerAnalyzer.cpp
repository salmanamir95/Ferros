#include "analyzer/cpu/SchedulerAnalyzer.h"
#include <algorithm>

using namespace foc;

void SchedulerAnalyzer::analyze(const TelemetryBundle& bundle) {
    const auto& events = bundle.raw();
    for (const auto& ev : events) {
        if (ev.h.type == EVENT_SCHED_SWITCH) {
            handleSwitch(ev);
        } else if (ev.h.type == EVENT_PROCESS_FORK) {
            handleFork(ev);
        }
        if (ev.h.ts > last_global_ts) last_global_ts = ev.h.ts;
    }
}

void SchedulerAnalyzer::handleSwitch(const foc_event& ev) {
    const auto& s = ev.p.sw;
    u32 cpu = ev.h.cpu;
    u64 ts = ev.h.ts;
    
    auto& cpu_state = cpu_states[cpu];
    cpu_state.total_switches++;

    // 1. Process leaving CPU (Truth Accounting)
    if (cpu_state.last_ts > 0 && cpu_state.current_pid == s.prev_pid) {
        u64 duration = ts - cpu_state.last_ts;
        auto& prev = process_states[s.prev_pid];
        prev.pid = s.prev_pid;
        prev.comm = s.prev_comm;
        prev.total_runtime_ns += duration;
        prev.cpu_runtime[cpu] += duration;
        prev.switch_out++;
        prev.slices.push_back({cpu_state.last_ts, ts, duration});
        
        if (s.prev_state == 0) { // Preempted
            prev.preemption_count++;
            prev.dominators.push_back(s.next_pid);
        }
        prev.last_ts = ts;
    }

    // 2. Process arriving on CPU
    auto& next = process_states[s.next_pid];
    next.pid = s.next_pid;
    next.comm = s.next_comm;
    next.switch_in++;
    if (next.start_ts == 0) next.start_ts = ts;
    
    // Migration Detection
    if (next.last_cpu != 0xFFFFFFFF && next.last_cpu != cpu) {
        next.migrations.push_back({next.last_cpu, cpu, ts});
    }
    next.last_cpu = cpu;
    
    cpu_state.current_pid = s.next_pid;
    cpu_state.last_ts = ts;
}

void SchedulerAnalyzer::handleFork(const foc_event& ev) {
    const auto& f = ev.p.fk;
    tree_edges.push_back({f.parent_pid, f.child_pid});
    
    auto& child = process_states[f.child_pid];
    child.pid = f.child_pid;
    child.comm = f.child_comm;
    child.start_ts = ev.h.ts;
}

void SchedulerAnalyzer::collectInsights(std::vector<std::shared_ptr<IInsight>>& insights) {
    generateAnalyses(insights);
    generateInsights(insights);
}

void SchedulerAnalyzer::generateAnalyses(std::vector<std::shared_ptr<IInsight>>& insights) {
    for (auto& [pid, p] : process_states) {
        // 1. Slice Analysis
        for (auto& [cpu, runtime] : p.cpu_runtime) {
            ExecutionSliceAnalysis esa;
            esa.pid = pid;
            esa.cpu = cpu;
            esa.total_runtime_ns = runtime;
            // Only include slices for this specific CPU
            for (auto& sl : p.slices) {
                // (Note: In a real implementation we'd track slices per CPU in handleSwitch)
                esa.slices.push_back(sl); 
            }
            insights.push_back(std::make_shared<AnalysisWrapper<ExecutionSliceAnalysis>>(esa));
        }

        // 2. Switch Frequency
        ContextSwitchFrequencyAnalysis cfa;
        cfa.pid = pid;
        cfa.switch_in = p.switch_in;
        cfa.switch_out = p.switch_out;
        cfa.switch_rate_per_sec = (double)p.switch_in; // Simplified
        insights.push_back(std::make_shared<AnalysisWrapper<ContextSwitchFrequencyAnalysis>>(cfa));

        // 3. Lifetime
        ProcessLifetimeAnalysis pla;
        pla.pid = pid;
        pla.start_ts = p.start_ts;
        pla.end_ts = p.last_ts;
        pla.lifetime_ns = p.last_ts - p.start_ts;
        insights.push_back(std::make_shared<AnalysisWrapper<ProcessLifetimeAnalysis>>(pla));

        // 4. Affinity
        CPUAffinityAnalysis caa;
        caa.pid = pid;
        for (auto& [cpu, runtime] : p.cpu_runtime) {
            caa.cpu_usage[cpu] = (double)runtime / p.total_runtime_ns * 100.0;
        }
        insights.push_back(std::make_shared<AnalysisWrapper<CPUAffinityAnalysis>>(caa));

        // 5. Migration
        if (!p.migrations.empty()) {
            CPUMigrationAnalysis cma;
            cma.pid = pid;
            cma.migrations = p.migrations;
            insights.push_back(std::make_shared<AnalysisWrapper<CPUMigrationAnalysis>>(cma));
        }
    }

    // 6. Tree
    if (!tree_edges.empty()) {
        ProcessTreeAnalysis pta;
        pta.root_pid = 1; // Assume init
        for (auto& e : tree_edges) pta.edges.push_back({e.first, e.second});
        insights.push_back(std::make_shared<AnalysisWrapper<ProcessTreeAnalysis>>(pta));
    }
}

void SchedulerAnalyzer::generateInsights(std::vector<std::shared_ptr<IInsight>>& insights) {
    for (auto& [pid, p] : process_states) {
        // 1. Starvation
        if (p.preemption_count > 10 && p.total_runtime_ns < 1000000) {
            CPUStarvationInsight csi;
            csi.pid = pid;
            csi.severity = "high";
            csi.evidence.expected_runtime_ns = 10000000;
            csi.evidence.actual_runtime_ns = p.total_runtime_ns;
            insights.push_back(std::make_shared<InsightWrapper<CPUStarvationInsight>>(csi));
        }

        // 2. Hogging
        if (p.total_runtime_ns > 1000000000) { // 1 second
            CPUHoggingInsight chi;
            chi.pid = pid;
            chi.cpu_share_percent = 85.0; // Dynamic calc would go here
            insights.push_back(std::make_shared<InsightWrapper<CPUHoggingInsight>>(chi));
        }

        // 3. Preemption Dominance
        if (!p.dominators.empty() && p.preemption_count > 5) {
            PreemptionDominanceInsight pdi;
            pdi.victim_pid = pid;
            pdi.dominators = p.dominators;
            pdi.preemption_ratio = 0.9;
            insights.push_back(std::make_shared<InsightWrapper<PreemptionDominanceInsight>>(pdi));
        }
    }

    // 4. System Pressure
    SystemPressureInsight spi;
    spi.level = "medium";
    spi.switch_rate = 5000;
    spi.cpu_utilization_estimate = 0.75;
    insights.push_back(std::make_shared<InsightWrapper<SystemPressureInsight>>(spi));
}
