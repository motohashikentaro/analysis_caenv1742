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

std::vector<TGraph*> RootfileAnalyzer::Waveform(int target_board, int target_ch, const std::vector<Long64_t>& target_evt, int range_start, int range_end){
    constexpr int kskipsample=10;
    constexpr int kpedestalcalc=50;

    std::vector<TGraph*> graphs;

    for(Long64_t evt : target_evt){
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
        TGraph* graph = new TGraph();
        for(int sample=0; sample<rd_.nsample; sample++){
            graph->SetPoint(sample, sample, rd_.ev_.amp[target_board][target_ch][sample] - pedestal);
        }
        graph->SetLineColor(TColor::GetColor("#008899"));
        graph->SetLineWidth(1);
        graph->GetYaxis()->SetRangeUser(-500, 100);
        graph->GetXaxis()->SetRangeUser(range_start, range_end);
        graph->GetXaxis()->SetTitle("Sample");
        graph->GetYaxis()->SetTitle("ADC - Pedestal [ADC]");
        graphs.push_back(graph);
    }

    return graphs;
}