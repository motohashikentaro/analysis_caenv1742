#include "./../include/analysis_process.h"
#include "./../include/rootfile_analyzer.h"
#include "./../include/save_objects.h"
#include "./../include/channel_map.h"

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

void AnalysisProcess::SimpleWaveform(int target_board, int target_ch){
    int loop_evt = 1000;

    std::vector<TGraph*> grs(loop_evt);

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);

    for(Long64_t evt=0; evt<loop_evt; evt++){
        rd_.tree_->GetEntry(evt);
        grs[evt] = new TGraph();
        for(int sample=0; sample<rd_.nsample; sample++){
            grs[evt]->SetPoint(sample, sample, rd_.ev_.amp[target_board][target_ch][sample]);
        }
        grs[evt]->SetLineWidth(2);
        grs[evt]->SetLineColor(kOrange+1);
        grs[evt]->GetYaxis()->SetRangeUser(-500, 100);
        if(evt==0) grs[evt]->Draw();
        if(evt>0) grs[evt]->Draw("same");
    }
    
    SaveObjects so(rd_);
    c1->SaveAs(so.MakeSavename("waveform", target_board, target_ch).c_str());

    for(auto* gr : grs){
        delete gr;
    }
}

void AnalysisProcess::SingleWaveform(int target_board, int target_ch, int target_evt){
    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    TGraph* gr = new TGraph();

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);
        if(evt == target_evt){   
            for(int sample=0; sample<rd_.nsample; sample++){
                gr->SetPoint(sample, sample, rd_.ev_.amp[target_board][target_ch][sample]);
            }
        }
        if(evt > target_evt) break;
    }

    gr->SetLineWidth(2);
    gr->SetLineColor(kOrange+1);
    gr->GetYaxis()->SetRangeUser(-500, 100);
    gr->Draw("AL");
    
    SaveObjects so(rd_);
    c1->SaveAs(so.MakeSavename("single_waveform", target_board, target_ch).c_str());

    delete gr;
}

void AnalysisProcess::SeparateWaveform(int target_board, int target_ch){
    int loop_evt = 500;
    int select_count = 0;
    int select_count_hit = 0;
    int select_count_noise = 0;

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    c1->Divide(2, 2);

    for(Long64_t evt=0; evt<loop_evt; evt++){
        rd_.tree_->GetEntry(evt);

        // pedestal calculate
        float pedestal = 0;
        std::array<float, 50> samples;
        for(int sample=0; sample<50; sample++){
            samples[sample] = rd_.ev_.amp[target_board][target_ch][sample];
        }
        std::sort(samples.begin(), samples.end());
        pedestal = (samples[24] + samples[25]) / 2.0;

        // evt search
        double min_adc = std::numeric_limits<double>::max();
        int min_adc_id = 0;
        for(int sample=0; sample<rd_.nsample; sample++){
            double adc_value = rd_.ev_.amp[target_board][target_ch][sample] - pedestal;
            if(adc_value < min_adc){
                min_adc = adc_value;
                min_adc_id = sample;
            }
        }

        if(50 < min_adc_id && min_adc < -100){  // hit selection
            if(select_count_hit >= 2) continue;
            std::cout << "hit evt = " << evt << std::endl;
            std::vector<TGraph*> grs(4);
            for(int i=0; i<4; i++) grs[i] = new TGraph();
            for(int sample=0; sample<rd_.nsample; sample++){
                double adc_value = rd_.ev_.amp[target_board][target_ch][sample] - pedestal;
                int gr_id = sample/256;
                grs[gr_id]->SetPoint(grs[gr_id]->GetN(), sample, adc_value);
            }
            for(int gr_id=0; gr_id<4; gr_id++){
                c1->cd(gr_id+1);
                grs[gr_id]->SetLineWidth(2);
                grs[gr_id]->GetYaxis()->SetRangeUser(-500, 100);
                if(select_count_hit == 0){
                    grs[gr_id]->SetLineColor(kOrange+1);
                }else{
                    grs[gr_id]->SetLineColor(kAzure+4);
                }
                if(select_count == 0){
                    grs[gr_id]->Draw("AL");
                }else{
                    grs[gr_id]->Draw("same");
                }
            }
            select_count++;
            select_count_hit++;
        }

        if(-100 < min_adc && min_adc < -50){  // noise selection
            if(select_count_noise >= 3) continue;
            std::cout << "noise evt = " << evt << std::endl;
            std::vector<TGraph*> grs(4);
            for(int i=0; i<4; i++) grs[i] = new TGraph();
            for(int sample=0; sample<rd_.nsample; sample++){
                double adc_value = rd_.ev_.amp[target_board][target_ch][sample] - pedestal;
                int gr_id = sample/256;
                grs[gr_id]->SetPoint(grs[gr_id]->GetN(), sample, adc_value);
            }
            for(int gr_id=0; gr_id<4; gr_id++){
                c1->cd(gr_id+1);
                grs[gr_id]->SetLineWidth(2);
                grs[gr_id]->GetYaxis()->SetRangeUser(-500, 100);
                if(select_count_noise == 0){
                    grs[gr_id]->SetLineColor(kGray);
                }else if(select_count_noise == 1){
                    grs[gr_id]->SetLineColor(kRed);
                }else if(select_count_noise == 2){
                    grs[gr_id]->SetLineColor(kBlack);
                }
                if(select_count == 0){
                    grs[gr_id]->Draw("AL");
                }else{
                    grs[gr_id]->Draw("same");
                }
            }
            select_count++;
            select_count_noise++;
        }
        if(select_count >= 5) break;
    }

    SaveObjects so(rd_);
    c1->SaveAs(so.MakeSavename("separate_waveform", target_board, target_ch).c_str());
}

void AnalysisProcess::MinAdcDistro(int target_board, int target_ch){
    TH1D* hist_min_adc = new TH1D("hist_min_adc", "Minimum ADC Distribution;ADC;Entries", 100, -700, 0);

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        // Calculate pedestal using the first 50 samples
        float pedestal = 0;
        std::array<float, 50> samples;
        for(int sample=0; sample<50; sample++){
            samples[sample] = rd_.ev_.amp[target_board][target_ch][sample];
        }
        std::sort(samples.begin(), samples.end());
        pedestal = (samples[24] + samples[25]) / 2.0;

        // main process
        double min_adc = std::numeric_limits<double>::max();
        for(int sample=0; sample<rd_.nsample; sample++){
            double adc_value = rd_.ev_.amp[target_board][target_ch][sample] - pedestal;
            min_adc = std::min(min_adc, adc_value);
        }
        hist_min_adc->Fill(min_adc);
    }

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    hist_min_adc->SetLineColor(kOrange+1);
    hist_min_adc->SetLineWidth(2);
    hist_min_adc->Draw();
    c1->SetLogy();

    SaveObjects so(rd_);
    c1->SaveAs(so.MakeSavename("min_adc_distro", target_board, target_ch).c_str());

    hist_min_adc->Delete();
}

void AnalysisProcess::Multiplicity(){
    TH1D* hist_multi = new TH1D("hist_multi", "Multiplicity", 33, -0.5, 32.5);
    int target_board = 1;
    double thres = -200;

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        // Calculate pedestal
        float pedestal[rd_.nch] = {0};
        std::array<float, 50> samples;
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
        // std::cout << nhit << std::endl;
        hist_multi->Fill(nhit);
    }

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    hist_multi->SetLineColor(kOrange+1);
    hist_multi->SetLineWidth(2);
    hist_multi->Draw();
    c1->SetLogy();
    c1->SaveAs((std::string("./../result/") + rd_.file_->GetName() + std::to_string(target_board) + std::string("_multiplicity.png")).c_str());

    hist_multi->Delete();
}

void AnalysisProcess::HitMap(){
    TH2D* front_hitmap = new TH2D("frontHitMap", "Front LGAD HitMap", 4, -0.5, 3.5, 4, -0.5, 3.5);
    TH2D* back_hitmap = new TH2D("backHitMap", "Back LGAD HitMap", 4, -0.5, 3.5, 4, -0.5, 3.5);
    int target_board = 1;
    double thres = -200;

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        // Calculate pedestal
        float pedestal[rd_.nch] = {0};
        std::array<float, 50> samples;
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

    c1->SaveAs((std::string("./../result/") + rd_.file_->GetName() + std::string("_pixel_hitmap.png")).c_str());
}

void AnalysisProcess::AveragePulse(){
    std::vector<double> avg_waveform(1024, 0.0);
    int nhit_evt = 0;
    int target_board = 1;
    int target_ch = 24;
    double thres = -200;
    TGraph* gr = new TGraph();

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);

        // Calcurate pedestal
        double pedestal = 0;
        std::array<float, 50> samples;
        for(int sample=0; sample<50; sample++){
            samples[sample] = rd_.ev_.amp[target_board][target_ch][sample];
        }
        std::sort(samples.begin(), samples.end());
        pedestal = (samples[24] + samples[25]) / 2.0;

        // main process
        double min_adc = std::numeric_limits<double>::max();
        for(int sample=0; sample<rd_.nsample; sample++){
            double adc_value = rd_.ev_.amp[target_board][target_ch][sample] - pedestal;
            min_adc = std::min(min_adc, adc_value);
        }
        if(min_adc < thres){
            for(int sample=0; sample<rd_.nsample; sample++){
                avg_waveform[sample] += rd_.ev_.amp[target_board][target_ch][sample] - pedestal;
            }
            nhit_evt++;
        }
    }

    for(int sample=0; sample<rd_.nsample; sample++){
        double avg_point = avg_waveform[sample] /= nhit_evt;
        gr->SetPoint(sample, sample, avg_point);
    }

    // std::cout << nhit_evt << "hit evt" << std::endl;

    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    gr->SetLineColor(kOrange);
    gr->SetLineWidth(2);
    gr->SetTitle("Average Waveform;sample;Average ADC");
    gr->Draw();
    c1->SaveAs((std::string("./../result/") + rd_.file_->GetName() + std::to_string(target_board) + std::string("_") + std::to_string(target_ch) + std::string("_avg_waveform.png")).c_str());
}
