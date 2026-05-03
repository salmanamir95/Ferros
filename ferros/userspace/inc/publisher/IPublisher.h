#pragma once

#include "telemetry/TelemetryBundle.h"

// Base publisher interface
class IPublisher
{
public:
    virtual ~IPublisher() = default;

    // Main publishing entry
    virtual void publish(
        const TelemetryBundle& bundle
    ) = 0;
};