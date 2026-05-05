#include "storage/FileStorage.h"
#include "common/Serialize.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iostream>

namespace fs = std::filesystem;

FileStorage::FileStorage(const std::string& dir) : directory(dir)
{
    if (!fs::exists(directory))
    {
        fs::create_directories(directory);
    }
}

void FileStorage::save(const std::vector<ProcessLifecycleInsight>& insights)
{
    if (insights.empty()) return;

    // Generate a timestamped filename
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    
    std::string filename = directory + "/telemetry_" + std::to_string(timestamp) + ".json";
    
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open())
    {
        std::cerr << "Failed to open " << filename << " for writing\n";
        return;
    }

    for (const auto& insight : insights)
    {
        outFile << serialize::toJson(insight).dump() << "\n";
    }
}
