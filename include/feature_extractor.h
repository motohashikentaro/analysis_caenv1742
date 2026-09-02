#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H

#include "./rootfile_reader.h"

#include <array>
#include <cstddef>

#include <RtypesCore.h>

struct ThresholdData{
    static constexpr std::array thresholds{30, 40, 70, 20};
    static constexpr size_t nthres = thresholds.size();
};

struct EvtFeature{
    Long64_t evt;

    int board;
    int ch;

    double pedestal;

    double peak_adc;  // minimum adc value
    int peak_time;  // minimum sample number

    std::array<double, ThresholdData::nthres> raise_times;  // time when the waveform crosses the threshold
    std::array<double, ThresholdData::nthres> fall_times;  // time when the waveform crosses the threshold
    std::array<double, ThresholdData::nthres> charges;  // integral of the waveform below the threshold
    std::array<double, ThresholdData::nthres> tots;  // time difference between raise_time and fall_time

    std::array<double, ThresholdData::nthres> raise_slopes; 
};

class FeatureExtractor{
    public:
        FeatureExtractor(RootData& rd): rd_(rd){};

        void FeatureExtractionCorrectThres();
        void FeatureExtraction();
        void FeatureExtractionOld();

    private:
        RootData& rd_;
};

#endif