#ifndef ANALYSIS_PROCESS_H
#define ANALYSIS_PROCESS_H

#include "./rootfile_analyzer.h"

struct EvtFeature{
    double evt;
    int board;
    int ch;
    double thres;
    double pedestal;
    int tot;
    double peak_adc;
    double peak_sample;
    double raise_sample;
    double fall_sample;
    double charge;
};

class AnalysisProcess{
    public:
        AnalysisProcess(RootData& rd): rd_(rd){};  // Constructor to initialize RootData reference
        ~AnalysisProcess(){};  // Destructor

        void HitSelection();

        void SimpleWaveform(int target_board, int target_ch);
        void SingleWaveform(int target_board, int target_ch, int target_evt);
        void SeparateWaveform(int target_board, int target_ch);

        void MinAdcDistro(int target_board, int target_ch);
        void Multiplicity();
        void HitMap();
        void AveragePulse();
    private:
        RootData& rd_;
};

#endif