#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include <csignal>
#include <getopt.h>
#include "workload_profiles.h"

std::atomic<bool> keep_running(true);

void signal_handler(int signum) {
    std::cout << "\n🛑 Signal received (" << signum << "), stopping..." << std::endl;
    keep_running = false;
}

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options]\n"
              << "Options:\n"
              << "  -m, --mode <mode>      Workload mode: sha256, mixed, burst, steady (default: mixed)\n"
              << "  -t, --threads <num>    Number of threads (default: hardware concurrency)\n"
              << "  -d, --duration <sec>   Duration in seconds (default: infinite)\n"
              << "  -g, --tag <string>     Instance tag for telemetry identification\n"
              << "  -h, --help             Show this help message\n";
}

int main(int argc, char* argv[]) {
    WorkloadMode mode = WorkloadMode::MIXED;
    int num_threads = std::thread::hardware_concurrency();
    int duration_seconds = -1;
    std::string tag = "default";

    static struct option long_options[] = {
        {"mode", required_argument, 0, 'm'},
        {"threads", required_argument, 0, 't'},
        {"duration", required_argument, 0, 'd'},
        {"tag", required_argument, 0, 'g'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "m:t:d:g:h", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'm':
                mode = stringToMode(optarg);
                if (mode == WorkloadMode::UNKNOWN) {
                    std::cerr << "❌ Unknown mode: " << optarg << std::endl;
                    return 1;
                }
                break;
            case 't':
                num_threads = std::stoi(optarg);
                break;
            case 'd':
                duration_seconds = std::stoi(optarg);
                break;
            case 'g':
                tag = optarg;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                return 1;
        }
    }

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << "🚀 Ferros Workload Generator [" << tag << "]" << std::endl;
    std::cout << "📊 Mode: " << modeToString(mode) << std::endl;
    std::cout << "🧵 Threads: " << num_threads << std::endl;
    if (duration_seconds > 0) {
        std::cout << "⏱️ Duration: " << duration_seconds << "s" << std::endl;
    } else {
        std::cout << "⏱️ Duration: Infinite" << std::endl;
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(WorkloadEngine::run, mode, i, std::ref(keep_running));
    }

    if (duration_seconds > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(duration_seconds));
        keep_running = false;
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    std::cout << "✅ Workload completed." << std::endl;
    return 0;
}
