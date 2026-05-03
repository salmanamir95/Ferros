#pragma once

#include <string>
#include <vector>

// Explicit JSON dependency
#include <nlohmann/json.hpp>

// Your insight type
#include "analyzer/cpu/pid_analyzer/ProcessLifecycleInsight.h"

// Optional: if you want to keep CPU marker hierarchy
#include "serialization/cpu/ICPUSerializer.h"

class ProcessLifecycleSerializer : public ICPUSerializer
{
public:
    std::string serialize(const std::vector<ProcessLifecycleInsight>& insights)
    {
        nlohmann::json arr = nlohmann::json::array();

        for (const auto& i : insights)
        {
            arr.push_back({
                {"pid", i.pid},
                {"first_seen", i.first_seen},
                {"last_seen", i.last_seen},
                {"event_count", i.event_count},
                {"exit_code", i.exit_code},
                {"exited", i.exited}
            });
        }

        return arr.dump();
    }
};