#pragma once

#include "publisher/IPublisher.h"
#include <string>

/**
 * @brief Publisher that sends data to the frontend (by writing to a JSON directory).
 */
class FrontendPublisher : public IPublisher
{
public:
    explicit FrontendPublisher(const std::string& dir);

    /**
     * @brief Publishes the serialized data string.
     */
    void publish(const std::string& data) override;

private:
    std::string directory;
    void updateIndex();
};
