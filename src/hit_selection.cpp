#include "./../include/hit_selection.h"
#include "./../include/rootfile_analyzer.h"
// #include "./../include/analysis_process.h"

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
    EvtFeature ef;

    TFile* fout = new TFile(("./../data/feature/feature_" + rd_.run_number_ + ".root").c_str(), "RECREATE");
    TTree* tree = new TTree("tree", "Waveform Features");

    tree->Branch("evt", &ef.evt);
    
    tree->Branch("board", &ef.board);
    tree->Branch("ch", &ef.ch);

    tree->Branch("pedestal", &ef.pedestal);

    tree->Branch("peak_adc", &ef.peak_adc);
    tree->Branch("peak_time", &ef.peak_time);

    for(size_t ithres=0; ithres<nthres; ithres++){
        tree->Branch(("raise_time_th" + std::to_string(thresholds[ithres])).c_str(), &ef.raise_times[ithres]);
        tree->Branch(("fall_time_th" + std::to_string(thresholds[ithres])).c_str(), &ef.fall_times[ithres]);
        tree->Branch(("charge_th" + std::to_string(thresholds[ithres])).c_str(), &ef.charges[ithres]);
        tree->Branch(("tot_th" + std::to_string(thresholds[ithres])).c_str(), &ef.tots[ithres]);
    }

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
                // use threshold <thresholds> in hit_selection.h
                ef.raise_times.fill(-1.0);
                ef.fall_times.fill(-1.0);
                ef.charges.fill(0.0);
                ef.tots.fill(0.0);

                for(size_t ithres=0; ithres<nthres; ithres++){
                    int thres = thresholds[ithres];
                    if(ef.peak_adc > -thres) continue;

                    // +------------+
                    // | raise time |
                    // +------------+
                    for(int sample=ef.peak_time; sample>0; sample--){
                        double adc1 = rd_.ev_.amp[board][ch][sample] - ef.pedestal;
                        double adc2 = rd_.ev_.amp[board][ch][sample - 1] -ef.pedestal;

                        if(adc1 < -thres && adc2 >= -thres){
                            double frac = (-thres - adc2) / (adc1 - adc2);
                            ef.raise_times[ithres] = (sample - 1) + frac;
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
                            ef.fall_times[ithres] = sample + frac;
                            break;
                        }
                    }

                    // +---------------------+
                    // | time over threshold |
                    // +---------------------+
                    if(ef.raise_times[ithres] >= 0 && ef.fall_times[ithres] >= 0){
                        ef.tots[ithres] = ef.fall_times[ithres] - ef.raise_times[ithres];
                    }

                    // +--------+
                    // | charge |
                    // +--------+
                    double start = ef.raise_times[ithres];
                    double end = ef.fall_times[ithres];
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
                    
                    ef.charges[ithres] = charge;
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

void HitSelection::HitExtraction(){
    constexpr int target_board = 1;
    constexpr int thresholds[] = {30, 50, 70, 90};

    // create root file
    TFile* fout = new TFile(("./../data/hit/hit_" + rd_.run_number_ + ".root").c_str(), "RECREATE");
    std::array<TDirectory*, nthres> dirs;
    for(size_t ithres=0; ithres<nthres; ithres++){
        dirs[ithres] = fout->mkdir(("thres_" + std::to_string(thresholds[ithres])).c_str());
    }

    // read root file
    TFile* fin = TFile::Open(("./../data/feature/feature_" + rd_.run_number_ + ".root").c_str(), "READ");
    TTree* tree = (TTree*)fin->Get("tree");

    EvtFeature ef;

    tree->SetBranchAddress("evt", &ef.evt);
    tree->SetBranchAddress("board", &ef.board);
    tree->SetBranchAddress("ch", &ef.ch);
    tree->SetBranchAddress("pedestal", &ef.pedestal);
    tree->SetBranchAddress("peak_adc", &ef.peak_adc);
    tree->SetBranchAddress("peak_time", &ef.peak_time);
    for(size_t ithres=0; ithres<nthres; ithres++){
        tree->SetBranchAddress(("raise_time_th" + std::to_string(thresholds[ithres])).c_str(), &ef.raise_times[ithres]);
        tree->SetBranchAddress(("fall_time_th" + std::to_string(thresholds[ithres])).c_str(), &ef.fall_times[ithres]);
        tree->SetBranchAddress(("charge_th" + std::to_string(thresholds[ithres])).c_str(), &ef.charges[ithres]);
        tree->SetBranchAddress(("tot_th" + std::to_string(thresholds[ithres])).c_str(), &ef.tots[ithres]);
    }

    Long64_t nentries = tree->GetEntries();

    for(Long64_t entry=0; entry<nentries; entry++){
        tree->GetEntry(entry);

        if(ef.board != target_board) continue;

        for(size_t ithres=0; ithres<nthres; ithres++){
            dirs[ithres]->cd();
            
        }
    }
}