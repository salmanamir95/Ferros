#pragma once

#include <vector>
#include "analyzer/cpu/pid_analyzers/ProcessLifecycleInsight.h"

class IStorage
{
public:
    virtual ~IStorage() = default;

    virtual void save(
        const std::vector<ProcessLifecycleInsight>& insights) = 0;
};
