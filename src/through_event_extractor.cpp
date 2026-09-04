#include "./../include/through_event_extractor.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/tracker.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TNamed.h>

void ThroughEventExtractor::ThroughEventExtraction(){
    
    const std::string hit_condition = hd_.tracker_condition_ + "-" + hd_.dut_condition_;
    const std::string reconstruction_condition = std::string(strip_condition_.name) + "-" + std::string(pixel_condition_.name);
    const std::filesystem::path output_dir = std::filesystem::path("./../data/through_event") / hd_.feature_condition_ / hit_condition / reconstruction_condition;

    std::filesystem::create_directories(output_dir);

    const std::filesystem::path output_path = output_dir / ("through_event_" + hd_.run_number_ + ".root");

    TFile* fout = new TFile(output_path.string().c_str(), "RECREATE");

    ThroughEvent te{};

    TTree* tree = new TTree("tree", "Through events");
    tree->Branch("evt", &te.evt);
    tree->Branch("strip_position_front_x", &te.strip_position_front_x);
    tree->Branch("strip_position_front_y", &te.strip_position_front_y);
    tree->Branch("strip_position_back_x", &te.strip_position_back_x);
    tree->Branch("strip_position_back_y", &te.strip_position_back_y);
    tree->Branch("dut_position_front_x", &te.dut_position_front_x);
    tree->Branch("dut_position_front_y", &te.dut_position_front_y);
    tree->Branch("dut_position_back_x", &te.dut_position_back_x);
    tree->Branch("dut_position_back_y", &te.dut_position_back_y);
    tree->Branch("extrapolated_front_x", &te.extrapolated_front_x);
    tree->Branch("extrapolated_front_y", &te.extrapolated_front_y);
    tree->Branch("extrapolated_back_x", &te.extrapolated_back_x);
    tree->Branch("extrapolated_back_y", &te.extrapolated_back_y);
    tree->Branch("slope_x", &te.slope_x);
    tree->Branch("slope_y", &te.slope_y);
    tree->Branch("n_hit_ch_front", &te.n_hit_ch_front);
    tree->Branch("n_hit_ch_back", &te.n_hit_ch_back);

    TNamed run_number("run_number", hd_.run_number_.c_str());
    TNamed feature_condition_metadata("feature_condition", hd_.feature_condition_.c_str());
    TNamed tracker_condition_metadata("tracker_condition", hd_.tracker_condition_.c_str());
    TNamed dut_condition_metadata("dut_condition", hd_.dut_condition_.c_str());
    TNamed strip_reconstruction_condition_metadata("strip_reconstruction_condition", strip_condition_.name);
    TNamed pixel_reconstruction_condition_metadata("pixel_reconstruction_condition", pixel_condition_.name);

    MatchedHitAnalyzer mha(hd_, strip_condition_, pixel_condition_);
    Tracker tracker;

    auto process_event = [&](Long64_t evt, const std::vector<EvtHit>& hits){
        const auto rhe = mha.Reconstruct(hits);

        if(!rhe) return;

        const TrackingResult tr = tracker.Track(*rhe);

        te.evt = evt;

        te.strip_position_front_x = rhe->strip_position_front_x;
        te.strip_position_front_y = rhe->strip_position_front_y;
        te.strip_position_back_x = rhe->strip_position_back_x;
        te.strip_position_back_y = rhe->strip_position_back_y;

        te.dut_position_front_x = rhe->dut_position_front.x;
        te.dut_position_front_y = rhe->dut_position_front.y;
        te.dut_position_back_x = rhe->dut_position_back.x;
        te.dut_position_back_y = rhe->dut_position_back.y;

        te.extrapolated_front_x = tr.extrapolated_front_x;
        te.extrapolated_front_y = tr.extrapolated_front_y;
        te.extrapolated_back_x = tr.extrapolated_back_x;
        te.extrapolated_back_y = tr.extrapolated_back_y;

        te.slope_x = tr.slope_x;
        te.slope_y = tr.slope_y;

        te.n_hit_ch_front = rhe->n_hit_ch_front;
        te.n_hit_ch_back = rhe->n_hit_ch_back;

        tree->Fill();
    };

    std::vector<EvtHit> hits;
    Long64_t previous_evt = -1;

    for(Long64_t entry=0; entry<hd_.nentries_; entry++){
        hd_.tree_->GetEntry(entry);

        const Long64_t current_evt = hd_.eh_.evt;

        if(previous_evt == -1) previous_evt = current_evt;

        if(current_evt != previous_evt){
            process_event(previous_evt, hits);
            hits.clear();
        }
        hits.push_back(hd_.eh_);
        previous_evt = current_evt;
    }

    if(!hits.empty()){
        process_event(previous_evt, hits);
    }

    fout->cd();
    tree->Write();
    run_number.Write();
    feature_condition_metadata.Write();
    tracker_condition_metadata.Write();
    dut_condition_metadata.Write();
    strip_reconstruction_condition_metadata.Write();
    pixel_reconstruction_condition_metadata.Write();

    fout->Close();
    delete fout;
}

void ThroughEventExtractor::ThroughStripEventExtraction(){
    
    const std::string hit_condition = hd_.tracker_condition_ + "-" + hd_.dut_condition_;
    const std::string reconstruction_condition = std::string(strip_condition_.name) + "-" + std::string(pixel_condition_.name);
    const std::filesystem::path output_dir = std::filesystem::path("./../data/through_event") / hd_.feature_condition_ / hit_condition / reconstruction_condition;

    std::filesystem::create_directories(output_dir);

    const std::filesystem::path output_path = output_dir / ("through_event_" + hd_.run_number_ + ".root");

    TFile* fout = new TFile(output_path.string().c_str(), "RECREATE");

    ThroughEvent te{};

    TTree* tree = new TTree("tree", "Through events");
    tree->Branch("evt", &te.evt);
    tree->Branch("strip_position_front_x", &te.strip_position_front_x);
    tree->Branch("strip_position_front_y", &te.strip_position_front_y);
    tree->Branch("strip_position_back_x", &te.strip_position_back_x);
    tree->Branch("strip_position_back_y", &te.strip_position_back_y);
    tree->Branch("dut_position_front_x", &te.dut_position_front_x);
    tree->Branch("dut_position_front_y", &te.dut_position_front_y);
    tree->Branch("dut_position_back_x", &te.dut_position_back_x);
    tree->Branch("dut_position_back_y", &te.dut_position_back_y);
    tree->Branch("extrapolated_front_x", &te.extrapolated_front_x);
    tree->Branch("extrapolated_front_y", &te.extrapolated_front_y);
    tree->Branch("extrapolated_back_x", &te.extrapolated_back_x);
    tree->Branch("extrapolated_back_y", &te.extrapolated_back_y);
    tree->Branch("slope_x", &te.slope_x);
    tree->Branch("slope_y", &te.slope_y);
    tree->Branch("n_hit_ch_front", &te.n_hit_ch_front);
    tree->Branch("n_hit_ch_back", &te.n_hit_ch_back);

    TNamed run_number("run_number", hd_.run_number_.c_str());
    TNamed feature_condition_metadata("feature_condition", hd_.feature_condition_.c_str());
    TNamed tracker_condition_metadata("tracker_condition", hd_.tracker_condition_.c_str());
    TNamed dut_condition_metadata("dut_condition", hd_.dut_condition_.c_str());
    TNamed strip_reconstruction_condition_metadata("strip_reconstruction_condition", strip_condition_.name);
    TNamed pixel_reconstruction_condition_metadata("pixel_reconstruction_condition", pixel_condition_.name);

    MatchedHitAnalyzer mha(hd_, strip_condition_, pixel_condition_);
    Tracker tracker;

    auto process_event = [&](Long64_t evt, const std::vector<EvtHit>& hits){
        const auto rhe = mha.StripReconstruct(hits);

        if(!rhe) return;

        const TrackingResult tr = tracker.Track(*rhe);

        te.evt = evt;

        te.strip_position_front_x = rhe->strip_position_front_x;
        te.strip_position_front_y = rhe->strip_position_front_y;
        te.strip_position_back_x = rhe->strip_position_back_x;
        te.strip_position_back_y = rhe->strip_position_back_y;

        te.dut_position_front_x = rhe->dut_position_front.x;
        te.dut_position_front_y = rhe->dut_position_front.y;
        te.dut_position_back_x = rhe->dut_position_back.x;
        te.dut_position_back_y = rhe->dut_position_back.y;

        te.extrapolated_front_x = tr.extrapolated_front_x;
        te.extrapolated_front_y = tr.extrapolated_front_y;
        te.extrapolated_back_x = tr.extrapolated_back_x;
        te.extrapolated_back_y = tr.extrapolated_back_y;

        te.slope_x = tr.slope_x;
        te.slope_y = tr.slope_y;

        te.n_hit_ch_front = rhe->n_hit_ch_front;
        te.n_hit_ch_back = rhe->n_hit_ch_back;

        tree->Fill();
    };

    std::vector<EvtHit> hits;
    Long64_t previous_evt = -1;

    for(Long64_t entry=0; entry<hd_.nentries_; entry++){
        hd_.tree_->GetEntry(entry);

        const Long64_t current_evt = hd_.eh_.evt;

        if(previous_evt == -1) previous_evt = current_evt;

        if(current_evt != previous_evt){
            process_event(previous_evt, hits);
            hits.clear();
        }
        hits.push_back(hd_.eh_);
        previous_evt = current_evt;
    }

    if(!hits.empty()){
        process_event(previous_evt, hits);
    }

    fout->cd();
    tree->Write();
    run_number.Write();
    feature_condition_metadata.Write();
    tracker_condition_metadata.Write();
    dut_condition_metadata.Write();
    strip_reconstruction_condition_metadata.Write();
    pixel_reconstruction_condition_metadata.Write();

    fout->Close();
    delete fout;    
}