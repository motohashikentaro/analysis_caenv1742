#include "./../include/hit_reader.h"
#include "./../include/hit_extractor.h"

#include <filesystem>

#include <TFile.h>
#include <TTree.h>

HitReader::HitReader(char* input_path){
    hd_.file_ = TFile::Open(input_path);

    std::filesystem::path path(input_path);
    hd_.filename_ = path.stem().string();

    hd_.run_number_condition_ = hd_.filename_.substr(4);

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