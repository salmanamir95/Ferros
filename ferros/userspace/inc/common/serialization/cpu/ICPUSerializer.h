#pragma once

#include <string>
#include "serialization/ISerializer.h"
#include <vector>

// Forward-declare or include your insight DTO
#include "analyzer/cpu/pid_analyzers/ProcessLifecycleInsight.h"

class ICPUSerializer : public ISerializer
{
public:
    virtual ~ICPUSerializer() = default;
};