#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * FINAL RESEARCH-GRADE LABEL ENGINE
 * Implements 100ms/50ms sliding window feature extraction
 * and dual-layer adversarial behavior analog mapping.
 */

struct FeatureVector {
    double cpu_utilization = 0.0;
    double context_switch_rate = 0.0;
    double execution_slice_variance = 0.0;
    int migration_count = 0;
    int process_count = 0;
};

class LabelEngine {
public:
    LabelEngine(const std::string& telemetry_path, const std::string& meta_path) {
        std::ifstream t_file(telemetry_path);
        t_file >> telemetry_data;
        
        std::ifstream m_file(meta_path);
        m_file >> ground_truth_meta;
    }

    void process() {
        if (telemetry_data.empty()) return;

        // 1. Identify Time Range
        uint64_t start_ts = 0xFFFFFFFFFFFFFFFF;
        uint64_t end_ts = 0;

        for (const auto& item : telemetry_data) {
            if (item.contains("slices")) {
                for (const auto& s : item["slices"]) {
                    uint64_t ts = s.value("start_ts", (uint64_t)0);
                    if (ts < start_ts) start_ts = ts;
                    if (ts > end_ts) end_ts = ts;
                }
            }
        }

        if (start_ts == 0xFFFFFFFFFFFFFFFF) return;

        // 2. Extract Windowed Features (100ms window, 50ms stride)
        uint64_t window_size_ns = 100000000; // 100ms
        uint64_t stride_ns = 50000000;       // 50ms

        for (uint64_t w_start = start_ts; w_start < end_ts; w_start += stride_ns) {
            extractWindow(w_start, w_start + window_size_ns);
        }

        // 3. Aggregate & Label
        generateFinalLabel();
    }

    void saveResult(const std::string& output_path) {
        json result;
        result["run_id"] = ground_truth_meta.value("name", "unknown");
        result["behavior_label"] = final_behavior;
        result["adversarial_behavior_analog_label"] = final_analog;
        result["confidence"] = confidence;
        result["decision_trace"] = decision_trace;
        
        result["feature_vector_summary"] = {
            {"avg_cpu", avg_features.cpu_utilization},
            {"avg_switch_rate", avg_features.context_switch_rate},
            {"avg_variance", avg_features.execution_slice_variance},
            {"total_migrations", total_migrations},
            {"max_process_count", max_procs}
        };

        std::ofstream out(output_path);
        out << result.dump(2);
    }

private:
    json telemetry_data;
    json ground_truth_meta;
    std::vector<FeatureVector> windows;
    
    // Final Results
    std::string final_behavior = "unknown";
    std::string final_analog = "unknown";
    double confidence = 0.0;
    std::vector<std::string> decision_trace;
    FeatureVector avg_features;
    int total_migrations = 0;
    int max_procs = 0;

    void extractWindow(uint64_t start, uint64_t end) {
        FeatureVector fv;
        std::vector<double> durations;
        uint64_t total_runtime = 0;
        int migrations = 0;
        std::set<int> pids;

        for (const auto& item : telemetry_data) {
            // Execution Slices
            if (item.contains("analysis_type") && item["analysis_type"] == "cpu_execution_slice") {
                if (item.contains("slices")) {
                    for (const auto& s : item["slices"]) {
                        uint64_t s_start = s.value("start_ts", (uint64_t)0);
                        
                        if (s_start >= start && s_start < end) {
                            uint64_t d = s.value("duration_ns", (uint64_t)0);
                            total_runtime += d;
                            durations.push_back((double)d);
                            pids.insert(item.value("pid", 0));
                        }
                    }
                }
            }
            
            // Migrations
            if (item.contains("analysis_type") && item["analysis_type"] == "cpu_migration") {
                if (item.contains("migrations")) {
                    for (const auto& m : item["migrations"]) {
                        uint64_t m_ts = m.value("ts", (uint64_t)0);
                        if (m_ts >= start && m_ts < end) migrations++;
                    }
                }
            }
        }

        fv.cpu_utilization = (double)total_runtime / (double)(end - start);
        fv.context_switch_rate = durations.size() / 0.1; // per second
        fv.migration_count = migrations;
        fv.process_count = pids.size();

        // Variance calculation
        if (!durations.empty()) {
            double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
            double mean = sum / durations.size();
            double sq_sum = 0;
            for (double d : durations) sq_sum += (d - mean) * (d - mean);
            fv.execution_slice_variance = std::sqrt(sq_sum / durations.size()) / 1000000.0; // in ms
        }

        windows.push_back(fv);
        total_migrations += migrations;
        if ((int)pids.size() > max_procs) max_procs = pids.size();
    }

    void generateFinalLabel() {
        if (windows.empty()) return;

        double sum_cpu = 0;
        double sum_switch = 0;
        double sum_var = 0;

        for (const auto& w : windows) {
            sum_cpu += w.cpu_utilization;
            sum_switch += w.context_switch_rate;
            sum_var += w.execution_slice_variance;
        }

        avg_features.cpu_utilization = sum_cpu / windows.size();
        avg_features.context_switch_rate = sum_switch / windows.size();
        avg_features.execution_slice_variance = sum_var / windows.size();

        // 🧠 Logic Triggers (Decision Trace)
        if (avg_features.cpu_utilization > 0.7) {
            decision_trace.push_back("high_sustained_cpu_detected");
            if (avg_features.execution_slice_variance < 5.0) {
                decision_trace.push_back("low_slice_variance_confirmed");
                final_behavior = "cpu_sustained";
                final_analog = "resource_exhaustion_behavior";
                confidence = 0.95;
            } else {
                final_behavior = "cpu_sustained";
                final_analog = "resource_exhaustion_behavior";
                confidence = 0.80;
            }
        } else if (avg_features.execution_slice_variance > 10.0) {
            decision_trace.push_back("high_burst_variance_detected");
            final_behavior = "cpu_bursty";
            final_analog = "bursty_resource_spike_behavior";
            confidence = 0.88;
        } else if (avg_features.context_switch_rate > 1000) {
            decision_trace.push_back("extreme_context_switch_rate");
            final_behavior = "scheduler_pressure";
            final_analog = "scheduler_pressure_behavior";
            confidence = 0.92;
        } else if (max_procs > 5) {
            decision_trace.push_back("process_tree_expansion_detected");
            final_behavior = "process_tree_growth";
            final_analog = "hierarchical_process_growth_behavior";
            confidence = 0.90;
        } else if (total_migrations > 20) {
            decision_trace.push_back("frequent_affinity_shifting_detected");
            final_behavior = "migration_heavy";
            final_analog = "cpu_migration_intensive_behavior";
            confidence = 0.85;
        } else {
            decision_trace.push_back("stable_scheduling_profile");
            final_behavior = "normal_baseline";
            final_analog = "normal_system_behavior";
            confidence = 0.98;
        }
    }
};

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: ./LabelEngine <telemetry.json> <meta.json> <output.json>" << std::endl;
        return 1;
    }

    try {
        LabelEngine engine(argv[1], argv[2]);
        engine.process();
        engine.saveResult(argv[3]);
        std::cout << "Labeling complete: " << argv[3] << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
