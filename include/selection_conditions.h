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

        static std::optional<size_t> LinearFunctionTrackerCondition(const EvtFeature& ef);
        static std::optional<size_t> CombinedFunctionTrackerCondition(const EvtFeature& ef);
        static std::optional<size_t> SlopeTotTrackerCondition(const EvtFeature& ef);

        static std::optional<size_t> StandardDutCondition(const EvtFeature& ef);
        static std::optional<size_t> StrictDutCondition(const EvtFeature& ef);
        static std::optional<size_t> TotPerPeakDutCondition(const EvtFeature& ef);
        static std::optional<size_t> LinearFunctionDutCondition(const EvtFeature& ef);
        static std::optional<size_t> CombinedFunctionDutCondition(const EvtFeature& ef);
        static std::optional<size_t> ForSubDutCondition(const EvtFeature& ef);
        static std::optional<size_t> SlopeTotDutCondition(const EvtFeature& ef);

        static const SelectionCondition LinearFunctionTracker;
        static const SelectionCondition CombinedFunctionTracker;
        static const SelectionCondition SlopeTotTracker;

        static const SelectionCondition StandardDut;
        static const SelectionCondition StrictDut;
        static const SelectionCondition TotPerPeakDut;
        static const SelectionCondition LinearFunctionDut;
        static const SelectionCondition CombinedFunctionDut;
        static const SelectionCondition ForSubDut;
        static const SelectionCondition SlopeTotDut;
};  

#endif