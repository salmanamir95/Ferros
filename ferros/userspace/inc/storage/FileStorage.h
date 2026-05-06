#pragma once

#include "publisher/IPublisher.h"
#include <string>

/**
 * @brief Publisher that saves data to local files.
 * Note: Inheriting from IPublisher for a unified pipeline.
 */
class FileStorage : public IPublisher
{
public:
    explicit FileStorage(const std::string& dir);

    /**
     * @brief Saves the serialized data string to a file.
     */
    void publish(const std::string& data) override;

private:
    std::string directory;
};
