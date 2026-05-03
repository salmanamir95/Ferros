#pragma once

#include "publisher/IPublisher.h"

class ICPUPublisher : public IPublisher
{
public:
    virtual ~ICPUPublisher() = default;

    void publish(
        const TelemetryBundle& bundle
    ) override
    {
        publishCPU(bundle);
    }

    virtual void publishCPU(
        const TelemetryBundle& bundle
    ) = 0;
};