#ifndef HIT_SELECTION_H
#define HIT_SELECTION_H

#include "./rootfile_analyzer.h"

struct EvtFeature{
    double evt;

    int board;
    int ch;

    double pedestal;

    double peak_adc;  // minimum adc value
    int peak_time;  // minimum sample number

    double raise_time_th30;  // time when the waveform crosses the threshold = -30
    double fall_time_th30;  // time when the waveform crosses the threshold = -30
    double charge_th30;  // integral of the waveform below the threshold = -30
    double tot_th30;  // time difference between raise_time_th30 and fall_time_th30

    double raise_time_th50;
    double fall_time_th50;
    double charge_th50;
    double tot_th50;

    double raise_time_th70;
    double fall_time_th70;
    double charge_th70;
    double tot_th70;

    double raise_time_th90;
    double fall_time_th90;
    double charge_th90;
    double tot_th90;
};

class HitSelection{
    public:
        HitSelection(RootData& rd): rd_(rd){};

        void FeatureExtraction();

    private:
        RootData& rd_;
};

#endif