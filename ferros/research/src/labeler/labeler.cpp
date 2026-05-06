#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <set>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * FERROS RESEARCH LABEL ENGINE
 * Bayesian classification with Mixture Ratio detection and stability smoothing.
 */

struct FeatureVector {
    double cpu_utilization = 0.0;
    double context_switch_rate = 0.0;
    double execution_slice_variance = 0.0;
    int migration_count = 0;
};

struct BayesianOutput {
    std::map<std::string, double> posteriors;
    double entropy = 0.0;
    std::string dominant;
};

class LabelEngine {
public:
    LabelEngine(const std::string& telemetry_path, const std::string& transition_log_path) {
        std::ifstream t_file(telemetry_path);
        t_file >> telemetry_data;
        std::ifstream l_file(transition_log_path);
        l_file >> transition_log;
        defineClassProfiles();
    }

    void process() {
        uint64_t g_start = 0xFFFFFFFFFFFFFFFF, g_end = 0;
        for (const auto& item : telemetry_data) {
            if (item.contains("slices")) {
                for (const auto& s : item["slices"]) {
                    uint64_t ts = s.value("start_ts", (uint64_t)0);
                    if (ts < g_start) g_start = ts;
                    if (ts > g_end) g_end = ts;
                }
            }
        }
        if (g_start == 0xFFFFFFFFFFFFFFFF) return;

        uint64_t window_size_ns = 100000000;
        uint64_t stride_ns = 50000000;
        for (uint64_t w_start = g_start; w_start < g_end; w_start += stride_ns) {
            processWindow(w_start, w_start + window_size_ns);
        }
    }

    void saveResult(const std::string& output_path) {
        std::ofstream file(output_path);
        file << window_results.dump(2);
    }

private:
    json telemetry_data;
    json transition_log;
    json window_results = json::array();
    struct Profile { double cpu, sw_rate, var; };
    std::map<std::string, Profile> class_profiles;

    void defineClassProfiles() {
        class_profiles["normal_system_behavior"] = {0.05, 100.0, 1.0};
        class_profiles["cpu_resource_pressure_behavior"] = {0.95, 200.0, 0.5};
        class_profiles["bursty_compute_behavior"] = {0.50, 500.0, 50.0};
        class_profiles["scheduler_contention_behavior"] = {0.30, 5000.0, 2.0};
    }

    void processWindow(uint64_t start, uint64_t end) {
        FeatureVector fv = extractFeatures(start, end);
        BayesianOutput bo = computeBayesian(fv);
        json mixture = computeMixture(start, end);

        json w_json;
        w_json["timestamp_range"] = {start, end};
        w_json["behavior_label"] = bo.dominant;
        w_json["mixture_ratio"] = mixture;
        w_json["bayesian_output"] = {
            {"posterior_distribution", bo.posteriors},
            {"confidence_entropy", bo.entropy}
        };
        double impact = std::min(1.0, (fv.cpu_utilization * 0.5) + (fv.context_switch_rate / 10000.0));
        w_json["system_impact"] = {{"score", impact}, {"category", impact > 0.4 ? "moderate" : "stable"}};
        window_results.push_back(w_json);
    }

    FeatureVector extractFeatures(uint64_t start, uint64_t end) {
        FeatureVector fv;
        uint64_t total_runtime = 0;
        std::vector<double> durations;
        for (const auto& item : telemetry_data) {
            if (item.value("analysis_type", "") == "cpu_execution_slice") {
                for (const auto& s : item["slices"]) {
                    uint64_t s_start = s["start_ts"], s_end = s["end_ts"];
                    if (s_end <= start || s_start >= end) continue;
                    uint64_t clip_s = std::max(s_start, start), clip_e = std::min(s_end, end);
                    total_runtime += (clip_e - clip_s);
                    durations.push_back((double)(clip_e - clip_s));
                }
            }
        }
        fv.cpu_utilization = (double)total_runtime / (end - start);
        fv.context_switch_rate = durations.size() / 0.1;
        if (!durations.empty()) {
            double sum = std::accumulate(durations.begin(), durations.end(), 0.0), mean = sum / durations.size(), sq_sum = 0;
            for (double d : durations) sq_sum += (d - mean) * (d - mean);
            fv.execution_slice_variance = std::sqrt(sq_sum / durations.size()) / 1000000.0;
        }
        return fv;
    }

    BayesianOutput computeBayesian(const FeatureVector& fv) {
        BayesianOutput bo;
        double total_likelihood = 0;
        std::map<std::string, double> likelihoods;
        for (auto const& [name, p] : class_profiles) {
            double d_cpu = std::pow(fv.cpu_utilization - p.cpu, 2) / 0.1;
            double d_sw = std::pow(std::log10(fv.context_switch_rate + 1) - std::log10(p.sw_rate + 1), 2) / 0.5;
            double d_var = std::pow(std::log10(fv.execution_slice_variance + 0.1) - std::log10(p.var + 0.1), 2) / 1.0;
            double L = std::max(1e-12, std::exp(-(d_cpu + d_sw + d_var)));
            likelihoods[name] = L;
            total_likelihood += L;
        }
        double max_p = -1.0;
        for (auto const& [name, L] : likelihoods) {
            double post = L / total_likelihood;
            bo.posteriors[name] = post;
            if (post > max_p) { max_p = post; bo.dominant = name; }
            if (post > 1e-6) bo.entropy -= post * std::log2(post);
        }
        return bo;
    }

    json computeMixture(uint64_t start, uint64_t end) {
        std::map<std::string, uint64_t> behavior_ns;
        for (const auto& r : transition_log) {
            uint64_t r_start = r["start_ns"], r_end = r_start + (uint64_t)r["duration_ms"] * 1000000;
            if (r_end <= start || r_start >= end) continue;
            uint64_t clip_s = std::max(r_start, start), clip_e = std::min(r_end, end);
            behavior_ns[r["behavior"]] += (clip_e - clip_s);
        }
        json mix = json::object();
        for (auto const& [name, ns] : behavior_ns) mix[name] = (double)ns / (end - start);
        return mix;
    }
};

int main(int argc, char** argv) {
    if (argc < 4) return 1;
    LabelEngine engine(argv[1], argv[2]);
    engine.process();
    engine.saveResult(argv[3]);
    return 0;
}
