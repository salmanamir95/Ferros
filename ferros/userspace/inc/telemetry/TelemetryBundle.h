#pragma once

#include <vector>
#include "events.h"

/**
 * @brief Container for the lossless truth stream.
 * 
 * In the new architecture, the bundle carries the raw event ledger
 * directly to the analyzers without intermediate normalization.
 */
class TelemetryBundle
{
public:
    TelemetryBundle() = default;
    ~TelemetryBundle() = default;

    /**
     * @brief Ingest a raw truth event from the kernel.
     */
    void addEvent(const foc_event& ev) { events.push_back(ev); }

    /**
     * @brief Access the full truth stream for analysis.
     */
    const std::vector<foc_event>& raw() const { return events; }

    /**
     * @brief Clear the ledger for the next pipeline step.
     */
    void clear() { events.clear(); }

private:
    std::vector<foc_event> events;
};