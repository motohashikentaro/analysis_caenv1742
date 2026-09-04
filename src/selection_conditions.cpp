#include "./../include/selection_conditions.h"
#include "./../include/feature_reader.h"
#include "./../include/feature_extractor.h"

#include <optional>

// Tracker selection conditions
// ===============================================================================================
std::optional<size_t> SelectionConditions::LinearFunctionTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 0;  // use threshold 30 for tracker

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;

    if(ef.tots[threshold_idx] < 0.04 * -ef.peak_adc) return std::nullopt;
    
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::CombinedFunctionTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 1;  // use threshold 30 for tracker

    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    // if(-ThresholdData::thresholds[threshold_idx] >= ef.peak_adc && ef.peak_adc > -50) is through
    if(ef.peak_adc <= -35 && ef.peak_adc > -100){
        if(ef.tots[threshold_idx] < 0.04 * -ef.peak_adc) return std::nullopt;
    }

    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::SlopeTotTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 1;  // use threshold 40 for DUT

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;    
    if(ef.tots[threshold_idx] < 5 && -ef.raise_slopes[threshold_idx] > 30) return std::nullopt;
    
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::CorrectThresTrackerCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 1;  // use threshold 40 for DUT

    if(ef.peak_adc >= -ThresholdData::thresholds[threshold_idx]) return std::nullopt;    
    if(ef.tots[threshold_idx] < 5 && -ef.raise_slopes[threshold_idx] > 30) return std::nullopt;
    if(ef.tots[threshold_idx] < 0.0) return std::nullopt;
    
    return std::make_optional(static_cast<size_t>(threshold_idx));
}


// DUT selection conditions
// ===============================================================================================
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

std::optional<size_t> SelectionConditions::CombinedFunctionDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 3;  // use threshold 30 for DUT

    if(ef.peak_time < 110 || ef.peak_time > 220) return std::nullopt;

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    // if(-ThresholdData::thresholds[threshold_idx] >= ef.peak_adc && ef.peak_adc > -50) is through
    if(ef.peak_adc <= -25 && ef.peak_adc > -80){
        if(ef.tots[threshold_idx] < 0.04 * -ef.peak_adc + 0.6) return std::nullopt;
    }

    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::ForSubDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 3;  // use threshold 20 for tracker

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;
    if(ef.peak_adc <= -30 && ef.peak_adc > -80){
        if(ef.charges[threshold_idx] < 2.5 * -ef.peak_adc -60) return std::nullopt;
    }
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::SlopeTotDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 3;  // use threshold 20 for DUT

    if(ef.peak_adc > -ThresholdData::thresholds[threshold_idx]) return std::nullopt;    
    if(ef.tots[threshold_idx] < 4 && -ef.raise_slopes[threshold_idx] > 30) return std::nullopt;
    
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::CorrectThresDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 3;  // use threshold 20 for DUT

    if(ef.peak_adc >= -ThresholdData::thresholds[threshold_idx]) return std::nullopt;    
    if(ef.tots[threshold_idx] < 4 && -ef.raise_slopes[threshold_idx] > 30) return std::nullopt;
    if(ef.tots[threshold_idx] < 0.0) return std::nullopt;
    
    return std::make_optional(static_cast<size_t>(threshold_idx));
}

std::optional<size_t> SelectionConditions::ForLast8chDutCondition(const EvtFeature& ef){
    constexpr size_t threshold_idx = 3;

    if(ef.peak_adc >= -ThresholdData::thresholds[threshold_idx]) return std::nullopt;    
    if(ef.tots[threshold_idx] < 3.3 && -ef.raise_slopes[threshold_idx] > 14) return std::nullopt;
    if(ef.tots[threshold_idx] < 0.0) return std::nullopt;
    
    return std::make_optional(static_cast<size_t>(threshold_idx));
    
}

std::optional<size_t> SelectionConditions::RejectAllDutCondition(const EvtFeature& ef){
    return std::nullopt;
}

const SelectionConditions::SelectionCondition SelectionConditions::LinearFunctionTracker{
    "Linear", 
    LinearFunctionTrackerCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::CombinedFunctionTracker{
    "Combined", 
    CombinedFunctionTrackerCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::SlopeTotTracker{
    "SlopeTot", 
    SlopeTotTrackerCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::StandardDut{
    "Standard", 
    StandardDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::StrictDut{
    "Strict", 
    StrictDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::TotPerPeakDut{
    "TotPerPeak", 
    TotPerPeakDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::LinearFunctionDut{
    "Linear", 
    LinearFunctionDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::CombinedFunctionDut{
    "Combined", 
    CombinedFunctionDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::ForSubDut{
    "ForSub", 
    ForSubDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::SlopeTotDut{
    "SlopeTot", 
    SlopeTotDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::CorrectThresTracker{
    "CorrectThresTracker", 
    CorrectThresTrackerCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::CorrectThresDut{
    "CorrectThresDut", 
    CorrectThresDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::ForLast8chDut{
    "ForLast8ch",
    ForLast8chDutCondition
};

const SelectionConditions::SelectionCondition SelectionConditions::RejectAllDut{
    "RejectAll",
    RejectAllDutCondition
};