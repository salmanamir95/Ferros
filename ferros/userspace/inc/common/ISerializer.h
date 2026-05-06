#pragma once

#include <string>
#include <memory>
#include "common/IInsight.h"

/**
 * @brief Interface for converting insights into a transportable format.
 */
class ISerializer {
public:
    virtual ~ISerializer() = default;

    /**
     * @brief Serializes a single insight into a string (e.g., JSON).
     */
    virtual std::string serialize(const IInsight& insight) = 0;
};
