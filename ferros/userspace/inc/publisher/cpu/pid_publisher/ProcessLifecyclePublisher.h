#pragma once
#include "publisher/cpu/ICPUPublisher.h"

class ProcessLifecyclePublisher
    : public ICPUPublisher
{
public:
    void publishCPU(
        const TelemetryBundle& bundle
    ) override;
};