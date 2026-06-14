#include <iostream>
#include <filesystem>
#include <algorithm>

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TApplication.h>

#include "./../include/rootfile_analyzer.h"

RootfileAnalyzer::RootfileAnalyzer(char* input_path){

    rd_.file_ = TFile::Open(input_path);

    rd_.tree_ = (TTree*)rd_.file_->Get("tree");

    rd_.tree_->SetBranchAddress("ev_id", &rd_.ev_.ev_id);
    for(int board=0; board<rd_.nboard; board++){
        for(int ch=0; ch<rd_.nch; ch++){
            rd_.tree_->SetBranchAddress(Form("amp_b%d_ch%02d", board, ch), rd_.ev_.amp[board][ch]);
        }
    }

    rd_.nentries_ = rd_.tree_->GetEntries();
}

RootfileAnalyzer::~RootfileAnalyzer(){
    if(rd_.file_){
        rd_.file_->Close();
        delete rd_.file_;
    }
}

void RootfileAnalyzer::EventLoop(){
    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        float pedestal[2][32] = {0};

        // Calculate pedestal using the first 50 samples
        for(int board=0; board<rd_.nboard; board++){
            for(int ch=0; ch<rd_.nch; ch++){
                std::array<UShort_t, 50> samples;
                for(int sample=0; sample<50; sample++){
                    samples[sample] = rd_.ev_.amp[board][ch][sample];
                }
                std::sort(samples.begin(), samples.end());
                pedestal[board][ch] = (samples[24] + samples[25]) / 2.0; // pedestal = median of the first 50 samples
            }
        }

        // main process
        for(int board=0; board<rd_.nboard; board++){
            for(int ch=0; ch<rd_.nch; ch++){
                for(int sample=0; sample<rd_.nsample; sample++){
                    rd_.ev_.amp[board][ch][sample] -= pedestal[board][ch];
                    // +--------------------------+
                    // | insert main process here |
                    // +--------------------------+
                }
            }
        }
    }
}