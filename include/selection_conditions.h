#ifndef SELECTION_CONDITIONS_H
#define SELECTION_CONDITIONS_H

#include "./feature_extractor.h"

#include <optional>

class SelectionConditions{
    public:
        using SelectionConditionFunction = std::optional<size_t> (*)(const EvtFeature& ef);

        struct SelectionCondition{
            const char* name;
            SelectionConditionFunction condition;
        };

        static std::optional<size_t> StandardTrackerCondition(const EvtFeature& ef);
        static std::optional<size_t> StrictTrackerCondition(const EvtFeature& ef);
        static std::optional<size_t> TotPerPeakTrackerCondition(const EvtFeature& ef);
        static std::optional<size_t> LinearFunctionTrackerCondition(const EvtFeature& ef);
        static std::optional<size_t> CombinedFunctionTrackerCondition(const EvtFeature& ef);

        static std::optional<size_t> StandardDutCondition(const EvtFeature& ef);
        static std::optional<size_t> StrictDutCondition(const EvtFeature& ef);
        static std::optional<size_t> TotPerPeakDutCondition(const EvtFeature& ef);
        static std::optional<size_t> LinearFunctionDutCondition(const EvtFeature& ef);
        static std::optional<size_t> CombinedFunctionDutCondition(const EvtFeature& ef);
};  

#endif