#include <iostream>
#include <filesystem>
#include <algorithm>
#include <limits>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TGraph.h>
#include <TCanvas.h>

#include "./../include/rootfile_analyzer.h"
#include "./../include/analysis_process.h"

void AnalysisProcess::MinAdcDistro(){
    TH1D* hist_min_adc = new TH1D("hist_min_adc", "hist_min_adc", 100, -1000, 0);
    TGraph* gr = new TGraph();
    int target_board = 1;
    int target_ch = 24;

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        // std::cout << "nentries" << rd_.nentries_ << std::endl;

        float pedestal = 0;

        // Calculate pedestal using the first 50 samples
        std::array<UShort_t, 50> samples;
        for(int sample=0; sample<50; sample++){
            samples[sample] = rd_.ev_.amp[target_board][target_ch][sample];
        }
        std::sort(samples.begin(), samples.end());
        pedestal = (samples[24] + samples[25]) / 2.0;

        // std::cout << "pedestal: " << pedestal << std::endl;

        // main process
        double min_adc = std::numeric_limits<double>::max();
        for(int sample=0; sample<rd_.nsample; sample++){
            double adc_value = rd_.ev_.amp[target_board][target_ch][sample] - pedestal;
            if(evt == 0) gr->SetPoint(gr->GetN(), sample, adc_value);
            min_adc = std::min(min_adc, adc_value);
        }
        // std::cout << "min_adc: " << min_adc << std::endl;
        hist_min_adc->Fill(min_adc);
    }

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    TCanvas* c2 = new TCanvas("c2", "c2", 800, 600);
    c1->cd();
    hist_min_adc->Draw();
    c1->SaveAs(("./../result/" + std::to_string(target_board) + "_" + std::to_string(target_ch) + "_min_adc_distro.png").c_str());
    c2->cd();
    gr->Draw("ALP");
    c2->SaveAs(("./../result/" + std::to_string(target_board) + "_" + std::to_string(target_ch) + "_waveform.png").c_str());

    hist_min_adc->Delete();
    c1->Delete();
    c2->Delete();
}

