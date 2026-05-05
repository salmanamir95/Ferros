#pragma once

#include "storage/IStorage.h"
#include <string>
#include <vector>

class FileStorage : public IStorage
{
public:
    explicit FileStorage(const std::string& dir);

    void save(
        const std::vector<ProcessLifecycleInsight>& insights) override;

private:
    std::string directory;
};
