#pragma once

#include <string>

/**
 * @brief Base interface for all analysis results (Insights).
 * Allows the pipeline to handle diverse insights polymorphically.
 */
class IInsight {
public:
    virtual ~IInsight() = default;

    /**
     * @brief Returns a unique string identifier for the insight type.
     */
    virtual std::string type() const = 0;
};
