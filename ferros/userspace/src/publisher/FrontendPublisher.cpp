#include "publisher/FrontendPublisher.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

FrontendPublisher::FrontendPublisher(const std::string& dir) : directory(dir)
{
    if (!fs::exists(directory))
    {
        fs::create_directories(directory);
    }
}

void FrontendPublisher::publish(const std::string& data)
{
    // 1. Update latest.json (Snapshot as JSON string)
    std::string latestPath = directory + "/latest.json";
    std::ofstream latestFile(latestPath);
    if (latestFile.is_open())
    {
        latestFile << data << std::endl;
    }

    // 2. Update index.json (List of all telemetry files)
    updateIndex();
}

void FrontendPublisher::updateIndex()
{
    nlohmann::json index = nlohmann::json::array();
    
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (entry.is_regular_file() && 
            entry.path().extension() == ".json" && 
            entry.path().filename() != "index.json" && 
            entry.path().filename() != "latest.json")
        {
            files.push_back(entry.path().filename().string());
        }
    }

    std::sort(files.begin(), files.end());

    for (const auto& file : files)
    {
        index.push_back(file);
    }

    std::string indexPath = directory + "/index.json";
    std::ofstream indexFile(indexPath);
    if (indexFile.is_open())
    {
        indexFile << index.dump(4) << std::endl;
    }
}
