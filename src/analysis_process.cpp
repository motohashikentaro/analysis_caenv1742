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
#include "./../include/channel_map.h"

void AnalysisProcess::MinAdcDistro(){
    TH1D* hist_min_adc = new TH1D("hist_min_adc", "hist_min_adc", 100, -1000, 0);
    TGraph* gr = new TGraph();
    int target_board = 1;
    int target_ch = 24;

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        // Calculate pedestal using the first 50 samples
        float pedestal = 0;
        std::array<UShort_t, 50> samples;
        for(int sample=0; sample<50; sample++){
            samples[sample] = rd_.ev_.amp[target_board][target_ch][sample];
        }
        std::sort(samples.begin(), samples.end());
        pedestal = (samples[24] + samples[25]) / 2.0;

        // main process
        double min_adc = std::numeric_limits<double>::max();
        for(int sample=0; sample<rd_.nsample; sample++){
            double adc_value = rd_.ev_.amp[target_board][target_ch][sample] - pedestal;
            if(evt == 0) gr->SetPoint(gr->GetN(), sample, adc_value);
            min_adc = std::min(min_adc, adc_value);
        }
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

void AnalysisProcess::Multiplicity(){
    TH1D* hist_multi = new TH1D("hist_multi", "hist_multi", 33, -0.5, 32.5);
    int target_board = 1;
    double thres = -300;

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        // Calculate pedestal
        float pedestal[rd_.nch] = {0};
        std::array<UShort_t, 50> samples;
        for(int ch=0; ch<rd_.nch; ch++){
            for(int sample=0; sample<50; sample++){
                samples[sample] = rd_.ev_.amp[target_board][ch][sample];
            }
            std::sort(samples.begin(), samples.end());
            pedestal[ch] = (samples[24] + samples[25]) / 2.0;
        }

        // main process
        int nhit = 0;
        for(int ch=0; ch<rd_.nch; ch++){
            double min_adc = std::numeric_limits<double>::max();
            for(int sample=0; sample<rd_.nsample; sample++){
                double adc_value = rd_.ev_.amp[target_board][ch][sample] - pedestal[ch];
                min_adc = std::min(min_adc, adc_value);
            }
            if(min_adc < thres) nhit++;
        }
        std::cout << nhit << std::endl;
        hist_multi->Fill(nhit);
    }

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    hist_multi->Draw();
    c1->SetLogy();
    c1->SaveAs(("./../result/" + std::to_string(target_board) + "_multiplicity.png").c_str());

    hist_multi->Delete();
}

void AnalysisProcess::HitMap(){
    TH2D* front_hitmap = new TH2D("frontHitMap", "HitMap", 4, -0.5, 3.5, 4, -0.5, 3.5);
    TH2D* back_hitmap = new TH2D("backHitMap", "HitMap", 4, -0.5, 3.5, 4, -0.5, 3.5);
    int target_board = 1;
    double thres = -300;

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        // Calculate pedestal
        float pedestal[rd_.nch] = {0};
        std::array<UShort_t, 50> samples;
        for(int ch=0; ch<rd_.nch; ch++){
            for(int sample=0; sample<50; sample++){
                samples[sample] = rd_.ev_.amp[target_board][ch][sample];
            }
            std::sort(samples.begin(), samples.end());
            pedestal[ch] = (samples[24] + samples[25]) / 2.0;
        }

        // main process

        for(int ch=0; ch<rd_.nch; ch++){
            double min_adc = std::numeric_limits<double>::max();
            for(int sample=0; sample<rd_.nsample; sample++){
                double adc_value = rd_.ev_.amp[target_board][ch][sample] - pedestal[ch];
                min_adc = std::min(min_adc, adc_value);
            }
            if(min_adc < thres){
                if(ch<16) front_hitmap->Fill(kChannelMap[0][ch].x, kChannelMap[0][ch].y);
                if(ch>=16) back_hitmap->Fill(kChannelMap[1][ch - 16].x, kChannelMap[1][ch - 16].y);
            }
        }
    }

    TCanvas* c1 = new TCanvas("c1", "c1", 1500, 900);
    c1->Divide(2,0);
    c1->cd(1);
    front_hitmap->Draw();
    c1->cd(2);
    back_hitmap->Draw();

    c1->SaveAs("./../result/pixel_hitmap.png");
}