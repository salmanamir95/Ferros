#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include "common/IInsight.h"

struct TimelineEvent {
    u64 ts;
    u32 cpu;
    std::string type; // "RUN", "STOP", "FORK"
    std::string detail;
};

struct ProcessInfo {
    u32 pid;
    std::string comm;
    u64 total_runtime_ns = 0;
    u32 slice_count = 0;
    u32 preemption_count = 0;
    u32 yield_count = 0;
    u32 sleep_count = 0;
    u32 blocked_count = 0;
    std::vector<TimelineEvent> timeline;
};

class SchedulerInsight : public IInsight {
public:
    std::string type() const override { return "SchedulerAnalysis"; }

    // Custom method for text reporting
    std::string toTextReport() const {
        std::stringstream ss;
        
        // A. Summary
        ss << "#### A. Summary\n";
        ss << "- Observed Processes: " << processes.size() << "\n";
        ss << "- Total Events Analyzed: " << total_events << "\n";
        ss << "- Detected Anomalies: " << anomalies.size() << "\n\n";

        // B. Process Timeline (Reconstructed)
        ss << "#### B. Process Timeline\n";
        for (const auto& [pid, info] : processes) {
            if (info.timeline.empty()) continue;
            ss << "- **PID " << pid << " (" << info.comm << ")**\n";
            ss << "  - Runtime: " << (info.total_runtime_ns / 1000000.0) << " ms (" << info.slice_count << " slices)\n";
            ss << "  - Behavior: " << info.preemption_count << " preemptions, " << info.yield_count << " yields\n";
            
            size_t start = info.timeline.size() > 5 ? info.timeline.size() - 5 : 0;
            for (size_t i = start; i < info.timeline.size(); ++i) {
                const auto& ev = info.timeline[i];
                ss << "    - [t=" << ev.ts << " cpu=" << ev.cpu << "] " << ev.type << ": " << ev.detail << "\n";
            }
        }
        ss << "\n";

        // C. Process Tree
        ss << "#### C. Process Tree\n";
        printTree(ss, 1, 0); 
        for (u32 root : tree_roots) {
            if (root != 1) printTree(ss, root, 0);
        }
        ss << "\n";

        // D. CPU Analysis
        ss << "#### D. CPU Analysis\n";
        for (const auto& [cpu, load] : cpu_load) {
            ss << "- CPU " << cpu << ": " << std::fixed << std::setprecision(2) << load << "% util (observed)\n";
        }
        ss << "\n";

        // E. Anomalies
        ss << "#### E. Anomalies\n";
        if (anomalies.empty()) {
            ss << "- No critical anomalies detected.\n";
        } else {
            for (const auto& a : anomalies) {
                ss << "- [!] " << a << "\n";
            }
        }

        return ss.str();
    }

    u32 total_events = 0;
    std::map<u32, ProcessInfo> processes;
    std::map<u32, std::vector<u32>> parent_to_children;
    std::set<u32> tree_roots;
    std::map<u32, double> cpu_load;
    std::vector<std::string> anomalies;

private:
    void printTree(std::stringstream& ss, u32 pid, int depth) const {
        if (processes.find(pid) == processes.end() && parent_to_children.find(pid) == parent_to_children.end()) return;
        
        for (int i = 0; i < depth; ++i) ss << "  ";
        ss << "`- " << pid;
        if (processes.count(pid)) ss << " [" << processes.at(pid).comm << "]";
        ss << "\n";

        if (parent_to_children.count(pid)) {
            for (u32 child : parent_to_children.at(pid)) {
                printTree(ss, child, depth + 1);
            }
        }
    }
};
