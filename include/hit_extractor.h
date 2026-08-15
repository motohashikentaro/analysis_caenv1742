#ifndef HIT_EXTRACTOR_H
#define HIT_EXTRACTOR_H

#include "./rootfile_reader.h"
#include "./feature_extractor.h"
#include "./selection_conditions.h"
#include "./feature_reader.h"

struct EvtHit{
    Long64_t evt;

    int board;
    int ch;

    double pedestal;

    double peak_adc;  // minimum adc value
    int peak_time;  // minimum sample number

    double raise_time;  // time when the waveform crosses the threshold
    double fall_time;  // time when the waveform crosses the threshold
    double charge;  // integral of the waveform below the threshold
    double tot;  // time difference between raise_time and fall_time
};

class HitExtractor{
    public:
        HitExtractor(FeatureData& fd): fd_(fd){};

        void HitExtraction(const SelectionConditions::SelectionCondition& tracker_condition, const SelectionConditions::SelectionCondition& dut_condition);

    private:
        FeatureData& fd_;
};

#endif