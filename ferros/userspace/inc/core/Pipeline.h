#pragma once

#include <vector>
#include <memory>
#include <string>

#include "analyzer/IAnalyzer.h"
#include "publisher/IPublisher.h"
#include "common/ISerializer.h"
#include "telemetry/TelemetryBundle.h"

/**
 * @brief Minimal Pipeline Orchestrator.
 * Manages the flow from Analyzers to Serializers and Publishers.
 */
class Pipeline {
public:
    Pipeline() = default;
    ~Pipeline() = default;

    /**
     * @brief Adds an analyzer to the pipeline.
     */
    void addAnalyzer(std::unique_ptr<IAnalyzer> analyzer) {
        analyzers.push_back(std::move(analyzer));
    }

    /**
     * @brief Adds a publisher to the pipeline.
     */
    void addPublisher(std::unique_ptr<IPublisher> publisher) {
        publishers.push_back(std::move(publisher));
    }

    /**
     * @brief Sets the serializer for the pipeline.
     */
    void setSerializer(std::unique_ptr<ISerializer> serializer) {
        this->serializer = std::move(serializer);
    }

    /**
     * @brief Executes one step of the pipeline.
     * @param bundle The telemetry data to process.
     */
    void step(const TelemetryBundle& bundle) {
        if (!serializer) return;

        std::vector<std::shared_ptr<IInsight>> insights;

        // 1. Analyze
        for (auto& analyzer : analyzers) {
            analyzer->run(bundle, insights);
        }

        // 2. Serialize and Publish
        for (const auto& insight : insights) {
            std::string serializedData = serializer->serialize(*insight);
            
            for (auto& publisher : publishers) {
                publisher->publish(serializedData);
            }
        }
    }

private:
    std::vector<std::unique_ptr<IAnalyzer>> analyzers;
    std::vector<std::unique_ptr<IPublisher>> publishers;
    std::unique_ptr<ISerializer> serializer;
};
