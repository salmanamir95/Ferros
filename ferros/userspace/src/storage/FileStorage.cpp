#include "storage/FileStorage.h"
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

void FileStorage::publish(const std::string& data)
{
    if (data.empty()) return;

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

    outFile << data << "\n";
}
