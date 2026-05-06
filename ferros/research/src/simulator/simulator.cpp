#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>
#include <cmath>
#include <sched.h>
#include <unistd.h>
#include <time.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * FERROS RESEARCH SIMULATOR
 * Professional, synchronized behavior generator using CLOCK_MONOTONIC.
 */

uint64_t get_mono_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

class BehaviorSimulator {
public:
    struct Regime {
        std::string name;
        int duration_ms;
    };

    void addRegime(const std::string& name, int duration_ms) {
        regimes.push_back({name, duration_ms});
    }

    void run() {
        std::cout << "🚀 Starting Research Simulator..." << std::endl;
        for (const auto& r : regimes) {
            uint64_t start_ns = get_mono_ns();
            transition_log.push_back({
                {"behavior", r.name},
                {"start_ns", start_ns},
                {"duration_ms", r.duration_ms}
            });
            std::cout << "🔄 Regime: " << r.name << " (" << r.duration_ms << "ms)" << std::endl;
            executeBehavior(r.name, r.duration_ms);
        }
        saveLog();
    }

private:
    std::vector<Regime> regimes;
    json transition_log = json::array();

    void executeBehavior(const std::string& name, int duration_ms) {
        uint64_t start_ns = get_mono_ns();
        uint64_t end_ns = start_ns + (uint64_t)duration_ms * 1000000;
        
        if (name == "normal_system_behavior") {
            usleep(duration_ms * 1000);
        } 
        else if (name == "cpu_resource_pressure_behavior") {
            while (get_mono_ns() < end_ns) {
                volatile double x = 0;
                for (int i = 0; i < 1000; i++) x += std::sqrt(i);
            }
        }
        else if (name == "bursty_compute_behavior") {
            while (get_mono_ns() < end_ns) {
                uint64_t burst_end = get_mono_ns() + 100000000;
                while (get_mono_ns() < burst_end) {
                    volatile int y = 0; for(int i=0; i<1000; i++) y++;
                }
                usleep(100000);
            }
        }
        else if (name == "scheduler_contention_behavior") {
            std::vector<std::thread> contention_threads;
            for (int i = 0; i < 8; i++) {
                contention_threads.emplace_back([&, end_ns]() {
                    while (get_mono_ns() < end_ns) {
                        sched_yield();
                    }
                });
            }
            for (auto& t : contention_threads) t.join();
        }
    }

    void saveLog() {
        std::ofstream file("research/transition_log.json");
        file << transition_log.dump(2);
    }
};

int main() {
    BehaviorSimulator sim;
    sim.addRegime("normal_system_behavior", 2000);
    sim.addRegime("cpu_resource_pressure_behavior", 3000);
    sim.addRegime("bursty_compute_behavior", 3000);
    sim.addRegime("scheduler_contention_behavior", 2000);
    sim.run();
    return 0;
}
