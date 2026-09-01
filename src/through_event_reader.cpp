#include "./../include/through_event_reader.h"

#include <string>
#include <filesystem>

#include <TFile.h>
#include <TTree.h>

ThroughEventReader::ThroughEventReader(const char* input_path){
    td_.file_ = TFile::Open(input_path, "READ");
    
    const std::filesystem::path path(input_path);
    td_.filename_ = path.stem().string();

    auto* run_number_metadata = (TNamed*)td_.file_->Get("run_number");
    auto* feature_condition_metadata = (TNamed*)td_.file_->Get("feature_condition");
    auto* tracker_condition_metadata = (TNamed*)td_.file_->Get("tracker_condition");
    auto* dut_condition_metadata = (TNamed*)td_.file_->Get("dut_condition");
    auto* strip_reconstruction_condition_metadata = (TNamed*)td_.file_->Get("strip_reconstruction_condition");
    auto* pixel_reconstruction_condition_metadata = (TNamed*)td_.file_->Get("pixel_reconstruction_condition");
    
    td_.run_number_ = run_number_metadata->GetTitle();
    td_.feature_condition_ = feature_condition_metadata->GetTitle();
    td_.tracker_condition_ = tracker_condition_metadata->GetTitle();
    td_.dut_condition_ = dut_condition_metadata->GetTitle();
    td_.strip_reconstruction_condition_ = strip_reconstruction_condition_metadata->GetTitle();
    td_.pixel_reconstruction_condition_ = pixel_reconstruction_condition_metadata->GetTitle();

    td_.tree_ = (TTree*)td_.file_->Get("tree");
    td_.tree_->SetBranchAddress("evt", &td_.te_.evt);
    td_.tree_->SetBranchAddress("strip_position_front_x", &td_.te_.strip_position_front_x);
    td_.tree_->SetBranchAddress("strip_position_front_y", &td_.te_.strip_position_front_y);
    td_.tree_->SetBranchAddress("strip_position_back_x", &td_.te_.strip_position_back_x);
    td_.tree_->SetBranchAddress("strip_position_back_y", &td_.te_.strip_position_back_y);
    td_.tree_->SetBranchAddress("dut_position_front_x", &td_.te_.dut_position_front_x);
    td_.tree_->SetBranchAddress("dut_position_front_y", &td_.te_.dut_position_front_y);
    td_.tree_->SetBranchAddress("dut_position_back_x", &td_.te_.dut_position_back_x);
    td_.tree_->SetBranchAddress("dut_position_back_y", &td_.te_.dut_position_back_y);
    td_.tree_->SetBranchAddress("extrapolated_front_x", &td_.te_.extrapolated_front_x);
    td_.tree_->SetBranchAddress("extrapolated_front_y", &td_.te_.extrapolated_front_y);
    td_.tree_->SetBranchAddress("extrapolated_back_x", &td_.te_.extrapolated_back_x);
    td_.tree_->SetBranchAddress("extrapolated_back_y", &td_.te_.extrapolated_back_y);
    td_.tree_->SetBranchAddress("slope_x", &td_.te_.slope_x);
    td_.tree_->SetBranchAddress("slope_y", &td_.te_.slope_y);
    td_.tree_->SetBranchAddress("n_hit_ch_front", &td_.te_.n_hit_ch_front);
    td_.tree_->SetBranchAddress("n_hit_ch_back", &td_.te_.n_hit_ch_back);

    td_.nentries_ = td_.tree_->GetEntries();
}

ThroughEventReader::~ThroughEventReader(){
    if(td_.file_){
        td_.file_->Close();
        delete td_.file_;
    }
}