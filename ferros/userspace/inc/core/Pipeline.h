#pragma once

#include <vector>
#include <memory>
#include <string>

#include "analyzer/IAnalyzer.h"
#include "publisher/IPublisher.h"
#include "common/ISerializer.h"
#include "telemetry/TelemetryBundle.h"
#include "core/DataAuditor.h"

/**
 * @brief Batch-Optimized Pipeline Orchestrator with Strict Pruning.
 */
class Pipeline {
public:
    Pipeline() = default;
    ~Pipeline() = default;

    void addAnalyzer(std::unique_ptr<IAnalyzer> analyzer) {
        analyzers.push_back(std::move(analyzer));
    }

    void addPublisher(std::unique_ptr<IPublisher> publisher) {
        publishers.push_back(std::move(publisher));
    }

    void setSerializer(std::unique_ptr<ISerializer> serializer) {
        this->serializer = std::move(serializer);
    }

    void step(const TelemetryBundle& bundle) {
        if (!serializer) return;

        std::vector<std::shared_ptr<IInsight>> raw_insights;
        for (auto& analyzer : analyzers) {
            analyzer->run(bundle, raw_insights);
        }

        std::vector<std::shared_ptr<IInsight>> clean_insights;
        DataAuditor auditor;
        auditor.audit(raw_insights, clean_insights);

        if (clean_insights.empty()) return;

        std::vector<std::string> serialized_blocks;
        for (const auto& ins : clean_insights) {
            std::string block = serializer->serialize(*ins);
            // SECONDARY SAFETY: Skip fallback empty objects
            if (block != "  {}") {
                serialized_blocks.push_back(block);
            }
        }

        if (serialized_blocks.empty()) return;

        // Construct final JSON array
        std::string batch = "[\n";
        for (size_t i = 0; i < serialized_blocks.size(); ++i) {
            batch += serialized_blocks[i];
            if (i < serialized_blocks.size() - 1) batch += ",\n";
        }
        batch += "\n]";

        for (auto& publisher : publishers) {
            publisher->publish(batch);
        }
    }

private:
    std::vector<std::unique_ptr<IAnalyzer>> analyzers;
    std::vector<std::unique_ptr<IPublisher>> publishers;
    std::unique_ptr<ISerializer> serializer;
};
