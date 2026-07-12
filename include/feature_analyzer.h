#ifndef FEATURE_ANALYZER_H
#define FEATURE_ANALYZER_H

#include "./../include/hit_selection.h"

#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>

struct FeatureData{
    EvtFeature ef_;

    TFile* file_;
    std::string filename_;
    std::string run_rumber_;
    TTree* tree_;
    Long64_t nentries_;
};

class FeatureAnalyzer{
    public:
        FeatureAnalyzer(char* input_path);
        ~FeatureAnalyzer();

        FeatureData& GetFeatureData(){return fd_;}

        TH1D* PeakDistro();
        std::array<TH1D*, nthres> ChargeDistro();
        TH1D* PeaktimeDistro();
        std::array<TH2D*, nthres> PeakVsCharge();
    private:
        FeatureData fd_;
};

#endif