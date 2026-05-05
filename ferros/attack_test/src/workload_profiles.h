#ifndef WORKLOAD_PROFILES_H
#define WORKLOAD_PROFILES_H

#include <string>
#include <atomic>
#include <cstdint>

enum class WorkloadMode {
    SHA256,
    MIXED,
    BURST,
    STEADY,
    UNKNOWN
};

WorkloadMode stringToMode(const std::string& mode_str);
std::string modeToString(WorkloadMode mode);

class WorkloadEngine {
public:
    static void run(WorkloadMode mode, int thread_id, std::atomic<bool>& keep_running);

private:
    static void runSha256(int thread_id, std::atomic<bool>& keep_running);
    static void runMixed(int thread_id, std::atomic<bool>& keep_running);
    static void runBurst(int thread_id, std::atomic<bool>& keep_running);
    static void runSteady(int thread_id, std::atomic<bool>& keep_running);
};

#endif // WORKLOAD_PROFILES_H
