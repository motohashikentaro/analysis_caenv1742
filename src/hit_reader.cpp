#include "./../include/hit_reader.h"
#include "./../include/hit_extractor.h"

#include <filesystem>
#include <stdexcept>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TNamed.h>

HitReader::HitReader(const char* input_path){
    hd_.file_ = TFile::Open(input_path);

    std::filesystem::path path(input_path);
    hd_.filename_ = path.stem().string();

    // metadata
    auto* run_number_metadata = (TNamed*)hd_.file_->Get("run_number");
    auto* feature_condition_metadata = (TNamed*)hd_.file_->Get("feature_condition");
    auto* tracker_condition_metadata = (TNamed*)hd_.file_->Get("tracker_condition");
    auto* dut_condition_metadata = (TNamed*)hd_.file_->Get("dut_condition");
    if(!run_number_metadata || !feature_condition_metadata || !tracker_condition_metadata || !dut_condition_metadata){
        hd_.file_->Close();
        delete hd_.file_;
        hd_.file_ = nullptr;
        throw std::runtime_error("Metadata not found in the ROOT file.");
    }
    hd_.run_number_ = run_number_metadata->GetTitle();
    hd_.feature_condition_ = feature_condition_metadata->GetTitle();
    hd_.tracker_condition_ = tracker_condition_metadata->GetTitle();
    hd_.dut_condition_ = dut_condition_metadata->GetTitle();

    hd_.tree_ = (TTree*)hd_.file_->Get("tree");

    hd_.tree_->SetBranchAddress("evt", &hd_.eh_.evt);

    hd_.tree_->SetBranchAddress("board", &hd_.eh_.board);
    hd_.tree_->SetBranchAddress("ch", &hd_.eh_.ch);

    hd_.tree_->SetBranchAddress("pedestal", &hd_.eh_.pedestal);

    hd_.tree_->SetBranchAddress("peak_adc", &hd_.eh_.peak_adc);
    hd_.tree_->SetBranchAddress("peak_time", &hd_.eh_.peak_time);

    hd_.tree_->SetBranchAddress("raise_time", &hd_.eh_.raise_time);
    hd_.tree_->SetBranchAddress("fall_time", &hd_.eh_.fall_time);
    hd_.tree_->SetBranchAddress("charge", &hd_.eh_.charge);
    hd_.tree_->SetBranchAddress("tot", &hd_.eh_.tot);

    hd_.tree_->SetBranchAddress("raise_slope", &hd_.eh_.raise_slope);

    hd_.nentries_ = hd_.tree_->GetEntries();
}

HitReader::~HitReader(){
    if(hd_.file_){
        hd_.file_->Close();
        delete hd_.file_;
    }
}