#include "./../include/selection_conditions.h"
#include "./../include/feature_reader.h"
#include "./../include/feature_extractor.h"

#include <optional>

std::optional<size_t> SelectionConditions::StandardTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for tracker

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 100 || ef.peak_time > 250) return std::nullopt;
    if(ef.tots[threshold_idx] <= 2.5) return std::nullopt;
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::StandardDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for DUT

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 100 || ef.peak_time > 250) return std::nullopt;
    if(ef.tots[threshold_idx] <= 2.5) return std::nullopt;
    return std::make_optional(static_cast<size_t>(threshold_idx));
}