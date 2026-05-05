#pragma once

#include "publisher/IPublisher.h"
#include <string>
#include <vector>

class FrontendPublisher : public IPublisher
{
public:
    explicit FrontendPublisher(const std::string& dir);

    void publish(
        const std::vector<ProcessLifecycleInsight>& insights) override;

private:
    std::string directory;
    void updateIndex();
};
