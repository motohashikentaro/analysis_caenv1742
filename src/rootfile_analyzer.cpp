#include "./../include/rootfile_analyzer.h"
#include "./../include/rootfile_reader.h"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <limits>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TLatex.h>

std::array<TGraph*, 1000> RootfileAnalyzer::Waveform(int target_board, int target_ch, int range_start, int range_end){
    constexpr int loop_evt = 1000;
    constexpr int kskipsample=10;
    constexpr int kpedestalcalc=50;

    std::array<TGraph*, loop_evt> graphs;

    for(Long64_t evt=0; evt<loop_evt; evt++){
        rd_.tree_->GetEntry(evt);
        
        // +---------------+
        // | pedestal calc |
        // +---------------+
        std::vector<double> ped_sample;
        double pedestal=0;
        for(int sample=kskipsample; sample<kpedestalcalc; sample++){
            ped_sample.push_back(rd_.ev_.amp[target_board][target_ch][sample]);
        }
        std::sort(ped_sample.begin(), ped_sample.end());
        pedestal=(ped_sample[19]+ped_sample[20])/2.0;

        // +----------+
        // | waveform |
        // +----------+
        graphs[evt] = new TGraph();
        for(int sample=0; sample<rd_.nsample; sample++){
            graphs[evt]->SetPoint(sample, sample, rd_.ev_.amp[target_board][target_ch][sample] - pedestal);
        }
        graphs[evt]->SetLineColor(TColor::GetColor("#008899"));
        graphs[evt]->SetLineWidth(1);
        graphs[evt]->GetYaxis()->SetRangeUser(-500, 100);
        graphs[evt]->GetXaxis()->SetRangeUser(range_start, range_end);
        graphs[evt]->GetXaxis()->SetTitle("Sample");
        graphs[evt]->GetYaxis()->SetTitle("ADC - Pedestal [ADC]");
    }

    return graphs;
}