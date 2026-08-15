#ifndef FEATURE_ANALYZER_H
#define FEATURE_ANALYZER_H

#include "./../include/feature_reader.h"

#include <string>

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

    private:
        FeatureData& fd_;
};

#endif