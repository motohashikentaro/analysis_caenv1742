#include "./../include/selection_conditions.h"
#include "./../include/feature_reader.h"
#include "./../include/feature_extractor.h"

#include <optional>

// Tracker selection conditions
std::optional<size_t> SelectionConditions::StandardTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for tracker

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 100 || ef.peak_time > 250) return std::nullopt;
    if(ef.tots[threshold_idx] <= 2.5) return std::nullopt;
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::StrictTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 1;  // use threshold 40 for tracker

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;
    if(ef.tots[threshold_idx] <= 2.5) return std::nullopt;
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::TotPerPeakTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for tracker

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;
    if(0.03 <= ef.tots[threshold_idx] / -ef.peak_adc && ef.tots[threshold_idx] / -ef.peak_adc < 0.04) return std::nullopt;
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::LinearFunctionTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for tracker

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;

    if(ef.tots[threshold_idx] < 0.04 * -ef.peak_adc) return std::nullopt;
    
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

// DUT selection conditions
std::optional<size_t> SelectionConditions::StandardDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for DUT

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 100 || ef.peak_time > 250) return std::nullopt;
    if(ef.tots[threshold_idx] <= 2.5) return std::nullopt;
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::StrictDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 1;  // use threshold 40 for DUT

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;
    if(ef.tots[threshold_idx] <= 2.5) return std::nullopt;
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::TotPerPeakDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for DUT

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;
    if(0.03 <= ef.tots[threshold_idx] / -ef.peak_adc && ef.tots[threshold_idx] / -ef.peak_adc < 0.04) return std::nullopt;
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::LinearFunctionDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for DUT

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;
    
    if(ef.tots[threshold_idx] < 0.04 * -ef.peak_adc) return std::nullopt;
    
    return std::make_optional(static_cast<size_t>(threshold_idx));
}