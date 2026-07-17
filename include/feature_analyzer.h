#ifndef FEATURE_ANALYZER_H
#define FEATURE_ANALYZER_H

#include "./../include/hit_selection.h"

#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>

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

        // TH1D* PeakDistro();
        // std::array<TH1D*, nthres> ChargeDistro();
        // TH1D* PeaktimeDistro();
        // std::array<TH2D*, nthres> PeakVsCharge();
        // TH1D* TotDistro();

        TH1D* PeakDistro(int ch);
        TH1D* ChargeDistro(int ch);
        TH1D* PeaktimeDistro(int ch);
        TH2D* PeakVsCharge(int ch);
        TH1D* TotDistro(int ch);

        void DrawHistInfo(TH1* hist,
                  int board,
                  int ch,
                  int threshold = 30,
                  double x = 0.58,
                  double y = 0.88);

    private:
        FeatureData fd_;
};

#endif