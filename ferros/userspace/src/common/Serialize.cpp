// Serialize.cpp

#include "common/Serialize.h"
#include <string>

namespace serialize
{

nlohmann::json toJson(
    const ProcessLifecycleInsight& insight)
{
    return {
        { "pid", insight.pid },
        { "comm", insight.comm },

        { "first_seen", insight.first_seen },
        { "last_seen", insight.last_seen },

        { "event_count", insight.event_count },

        { "exit_code", insight.exit_code },
        { "exited", insight.exited }
    };
}

nlohmann::json toJson(
    const std::vector<ProcessLifecycleInsight>& insights)
{
    nlohmann::json arr = nlohmann::json::array();

    for (const auto& insight : insights)
    {
        arr.push_back(toJson(insight));
    }

    return arr;
}

std::string toPrettyString(
    const ProcessLifecycleInsight& insight)
{
    return toJson(insight).dump(4);
}

std::string toPrettyString(
    const std::vector<ProcessLifecycleInsight>& insights)
{
    return toJson(insights).dump(4);
}

std::string toCompactString(
    const std::vector<ProcessLifecycleInsight>& insights)
{
    return toJson(insights).dump();
}

}