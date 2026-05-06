#pragma once

#include <string>

/**
 * @brief Interface for publishing serialized insight data.
 */
class IPublisher
{
public:
    virtual ~IPublisher() = default;

    /**
     * @brief Publishes the serialized data to the target destination.
     * @param data Serialized insight data (e.g., JSON string).
     */
    virtual void publish(const std::string& data) = 0;
};