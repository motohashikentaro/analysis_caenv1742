#ifndef ANALYSIS_PROCESS_H
#define ANALYSIS_PROCESS_H

#include "./rootfile_analyzer.h"

#include <TGraph.h>

class AnalysisProcess{
    public:
        AnalysisProcess(RootData& rd): rd_(rd){};  // Constructor to initialize RootData reference
        ~AnalysisProcess(){};  // Destructor

        std::array<TGraph*, 500> Waveform(int target_board, int target_ch);
        // void SimpleWaveform(int target_board, int target_ch);
        // void SingleWaveform(int target_board, int target_ch, int target_evt);
        // void SeparateWaveform(int target_board, int target_ch);

        // void MinAdcDistro(int target_board, int target_ch);
        // void Multiplicity();
        // void HitMap();
        // void AveragePulse();
    private:
        RootData& rd_;
};

#endif