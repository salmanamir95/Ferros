#pragma once
#include "common/IInsight.h"
#include "analyzer/cpu/models/AnalysisModels.h"
#include "analyzer/cpu/models/InsightModels.h"

namespace foc {

// Wrapper classes to make models polymorphic
template<typename T>
class AnalysisWrapper : public IInsight {
public:
    T data;
    AnalysisWrapper(const T& d) : data(d) {}
    std::string type() const override { return data.analysis_type; }
};

template<typename T>
class InsightWrapper : public IInsight {
public:
    T data;
    InsightWrapper(const T& d) : data(d) {}
    std::string type() const override { return data.insight_type; }
};

} // namespace foc
