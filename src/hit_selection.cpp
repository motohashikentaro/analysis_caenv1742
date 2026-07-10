#include "./../include/hit_selection.h"
#include "./../include/rootfile_analyzer.h"
#include "./../include/analysis_process.h"

#include <iostream>
#include <filesystem>
#include <algorithm>
#include <limits>
#include <cmath>

#include <TFile.h>
#include <TTree.h>

void HitSelection::FeatureExtraction(){
    constexpr int kskipsample=10;
    constexpr int kpedestalcalc=50;
    constexpr int thresholds[] = {30, 50, 70, 90};
    EvtFeature ef;

    TFile* fout = new TFile(("./../data/feature/feature_" + rd_.run_number_ + ".root").c_str(), "RECREATE");
    TTree* tree = new TTree("tree", "Waveform Features");

    tree->Branch("evt", &ef.evt);
    
    tree->Branch("board", &ef.board);
    tree->Branch("ch", &ef.ch);

    tree->Branch("pedestal", &ef.pedestal);

    tree->Branch("peak_adc", &ef.peak_adc);
    tree->Branch("peak_time", &ef.peak_time);

    tree->Branch("raise_time_th30", &ef.raise_time_th30);
    tree->Branch("fall_time_th30", &ef.fall_time_th30);
    tree->Branch("charge_th30", &ef.charge_th30);
    tree->Branch("tot_th30", &ef.tot_th30);

    tree->Branch("raise_time_th50", &ef.raise_time_th50);
    tree->Branch("fall_time_th50", &ef.fall_time_th50);
    tree->Branch("charge_th50", &ef.charge_th50);
    tree->Branch("tot_th50", &ef.tot_th50);

    tree->Branch("raise_time_th70", &ef.raise_time_th70);
    tree->Branch("fall_time_th70", &ef.fall_time_th70);
    tree->Branch("charge_th70", &ef.charge_th70);
    tree->Branch("tot_th70", &ef.tot_th70);

    tree->Branch("raise_time_th90", &ef.raise_time_th90);
    tree->Branch("fall_time_th90", &ef.fall_time_th90);
    tree->Branch("charge_th90", &ef.charge_th90);
    tree->Branch("tot_th90", &ef.tot_th90);

    for(Long64_t evt=0; evt<rd_.nentries_; evt++){
        rd_.tree_->GetEntry(evt);
        ef.evt=evt;

        for(int board=0; board<rd_.nboard; board++){
            ef.board=board;

            for(int ch=0; ch<rd_.nch; ch++){
                ef.ch=ch;

                // +---------------+
                // | pedestal calc |
                // +---------------+
                std::vector<double> ped_sample;
                for(int sample=kskipsample; sample<kpedestalcalc; sample++){
                    ped_sample.push_back(rd_.ev_.amp[board][ch][sample]);
                }
                std::sort(ped_sample.begin(), ped_sample.end());
                ef.pedestal=(ped_sample[19]+ped_sample[20])/2.0;

                // +------------+
                // | peak value |
                // +------------+
                ef.peak_time=-1;
                ef.peak_adc=std::numeric_limits<double>::max();
                for(int sample=0; sample<rd_.nsample; sample++){
                    double adc = rd_.ev_.amp[board][ch][sample] - ef.pedestal;
                    if(adc < ef.peak_adc){
                        ef.peak_adc=adc;
                        ef.peak_time=sample;
                    }
                }

                // calculate feature that depends on threshold
                ef.raise_time_th30=-1;
                ef.fall_time_th30=-1;
                ef.charge_th30=0.0;
                ef.tot_th30=0.0;

                ef.raise_time_th50=-1;
                ef.fall_time_th50=-1;
                ef.charge_th50=0.0;
                ef.tot_th50=0.0;

                ef.raise_time_th70=-1;
                ef.fall_time_th70=-1;
                ef.charge_th70=0.0;
                ef.tot_th70=0.0;

                ef.raise_time_th90=-1;
                ef.fall_time_th90=-1;
                ef.charge_th90=0.0;
                ef.tot_th90=0.0;
                for(int thres : thresholds){
                    if(ef.peak_adc > -thres) continue;

                    // +------------+
                    // | raise time |
                    // +------------+
                    for(int sample=ef.peak_time; sample>0; sample--){
                        double adc1 = rd_.ev_.amp[board][ch][sample] - ef.pedestal;
                        double adc2 = rd_.ev_.amp[board][ch][sample - 1] -ef.pedestal;

                        if(adc1 < -thres && adc2 >= -thres){
                            double frac = (-thres - adc2) / (adc1 - adc2);
                            if(thres == 30) ef.raise_time_th30 = (sample - 1) + frac;
                            if(thres == 50) ef.raise_time_th50 = (sample - 1) + frac;
                            if(thres == 70) ef.raise_time_th70 = (sample - 1) + frac;
                            if(thres == 90) ef.raise_time_th90 = (sample - 1) + frac;
                            break;
                        }
                    }

                    // +-----------+
                    // | fall time |
                    // +-----------+
                    for(int sample=ef.peak_time; sample<rd_.nsample - 1; sample++){
                        double adc1 = rd_.ev_.amp[board][ch][sample] - ef.pedestal;
                        double adc2 = rd_.ev_.amp[board][ch][sample + 1] - ef.pedestal;

                        if(adc1 < -thres && adc2 >= -thres){
                            double frac = (-thres - adc1) / (adc2 - adc1);
                            if(thres == 30) ef.fall_time_th30 = sample + frac;
                            if(thres == 50) ef.fall_time_th50 = sample + frac;
                            if(thres == 70) ef.fall_time_th70 = sample + frac;
                            if(thres == 90) ef.fall_time_th90 = sample + frac;
                            break;
                        }
                    }

                    // +---------------------+
                    // | time over threshold |
                    // +---------------------+
                    if(ef.raise_time_th30 >= 0 && ef.fall_time_th30 >= 0) ef.tot_th30 = ef.fall_time_th30 - ef.raise_time_th30;
                    if(ef.raise_time_th50 >= 0 && ef.fall_time_th50 >= 0) ef.tot_th50 = ef.fall_time_th50 - ef.raise_time_th50;
                    if(ef.raise_time_th70 >= 0 && ef.fall_time_th70 >= 0) ef.tot_th70 = ef.fall_time_th70 - ef.raise_time_th70;
                    if(ef.raise_time_th90 >= 0 && ef.fall_time_th90 >= 0) ef.tot_th90 = ef.fall_time_th90 - ef.raise_time_th90;

                    // +--------+
                    // | charge |
                    // +--------+
                    double start = -1.0;
                    double end = -1.0;
                    switch (thres){
                    case 30: start = ef.raise_time_th30; end = ef.fall_time_th30; break;
                    case 50: start = ef.raise_time_th50; end = ef.fall_time_th50; break;
                    case 70: start = ef.raise_time_th70; end = ef.fall_time_th70; break;
                    case 90: start = ef.raise_time_th90; end = ef.fall_time_th90; break;
                    default: break;
                    }
                    if(start < 0.0 || end < 0.0) continue;
                    double charge = 0.0;
                    int idx_start = static_cast<int>(std::ceil(start));
                    int idx_end = static_cast<int>(std::floor(end));
                    // start edge charge
                    double w_start = idx_start - start;
                    double h_start_edge = 0.0;
                    double h_start_idx = (-thres) - (rd_.ev_.amp[board][ch][idx_start] - ef.pedestal);
                    charge = charge + (w_start*(h_start_edge + h_start_idx) / 2.0);
                    // middle charge
                    for(int sample=idx_start; sample<idx_end; sample++){
                        double h1 = (-thres) - (rd_.ev_.amp[board][ch][sample] - ef.pedestal);
                        double h2 = (-thres) - (rd_.ev_.amp[board][ch][sample + 1] - ef.pedestal);
                        charge = charge + ((h1 + h2) / 2.0);
                    }
                    // end edge charge
                    double w_end = end - idx_end;
                    double h_end_edge = 0.0;
                    double h_end_idx = (-thres) - (rd_.ev_.amp[board][ch][idx_end] - ef.pedestal);
                    charge = charge + (w_end*(h_end_edge + h_end_idx) / 2.0);
                    
                    switch (thres){
                    case 30: ef.charge_th30 = charge; break;
                    case 50: ef.charge_th50 = charge; break;
                    case 70: ef.charge_th70 = charge; break;
                    case 90: ef.charge_th90 = charge; break;
                    default: break;
                    }
                }  // end of threshold loop
                tree->Fill();
            }  // end of ch loop
        }  // end of board loop
    }  // end of evt loop

    fout->cd();

    tree->Write();

    fout->Close();

    delete fout;
}