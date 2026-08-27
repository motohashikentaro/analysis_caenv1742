#ifndef FEATURE_ANALYZER_H
#define FEATURE_ANALYZER_H

#include "./../include/feature_reader.h"

#include <string>
#include <functional>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>

class FeatureAnalyzer{
    public:
        FeatureAnalyzer(FeatureData& fd): fd_(fd){};  // Constructor to initialize FeatureData reference

        TH1D* PeakDistro(int board, int ch);
        TH1D* PeaktimeDistro(int board, int ch);
        TH1D* ChargeDistro(int board, int ch, int target_threshold);
        TH2D* PeakVsCharge(int board, int ch, int target_threshold);
        TH1D* TotDistro(int board, int ch, int target_threshold);
        TH2D* PeakVsTot(int board, int ch, int target_threshold);
        TH1D* ChargePerPeakDistro(int board, int ch, int target_threshold);
        TH1D* TotPerPeakDistro(int board, int ch, int target_threshold);
        TH1D* ChargeMinusAlphaAdcDistro(int board, int ch, int target_threshold);
        TH1D* TotMinusAlphaAdcDistro(int board, int ch, int target_threshold);
        TH1D* SlopeDistro(int board, int ch, int target_threshold);
        TH2D* TotVsSlope(int board, int ch, int target_threshold);
        TH2D* PeakVsSlope(int board, int ch, int target_threshold);
        TH2D* ChargeVsSlope(int board, int ch, int target_threshold);
        TH2D* PeaktimeVsSlope(int board, int ch, int target_threshold);
        TH2D* PeakVsPeaktime(int board, int ch, int target_threshold);

        std::vector<Long64_t> SelectEvents(int board, int ch, size_t max_events, const std::function<bool(const EvtFeature&)>& condition);

    private:
        FeatureData& fd_;
};

#endif