#ifndef HIT_SELECTION_H
#define HIT_SELECTION_H

#include "./rootfile_analyzer.h"

#include <array>

inline constexpr std::array<int, 4> thresholds = {30, 50, 70, 90};
inline constexpr size_t nthres = thresholds.size();

struct EvtFeature{
    double evt;

    int board;
    int ch;

    double pedestal;

    double peak_adc;  // minimum adc value
    int peak_time;  // minimum sample number

    std::array<double, nthres> raise_times;  // time when the waveform crosses the threshold
    std::array<double, nthres> fall_times;  // time when the waveform crosses the threshold
    std::array<double, nthres> charges;  // integral of the waveform below the threshold
    std::array<double, nthres> tots;  // time difference between raise_time and fall_time
};

class HitSelection{
    public:
        HitSelection(RootData& rd): rd_(rd){};

        void FeatureExtraction();
        void HitExtraction();

    private:
        RootData& rd_;
};

#endif