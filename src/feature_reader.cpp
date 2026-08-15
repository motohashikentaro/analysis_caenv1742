#include "./../include/feature_reader.h"
#include "./../include/feature_extractor.h"

#include <filesystem>

#include <TFile.h>
#include <TTree.h>

FeatureReader::FeatureReader(char* input_path){
    fd_.file_ = TFile::Open(input_path);

    std::filesystem::path path(input_path);
    fd_.filename_ = path.stem().string();

    fd_.run_number_ = fd_.filename_.substr(8);

    fd_.tree_ = (TTree*)fd_.file_->Get("tree");

    fd_.tree_->SetBranchAddress("evt", &fd_.ef_.evt);

    fd_.tree_->SetBranchAddress("board", &fd_.ef_.board);
    fd_.tree_->SetBranchAddress("ch", &fd_.ef_.ch);

    fd_.tree_->SetBranchAddress("pedestal", &fd_.ef_.pedestal);

    fd_.tree_->SetBranchAddress("peak_adc", &fd_.ef_.peak_adc);
    fd_.tree_->SetBranchAddress("peak_time", &fd_.ef_.peak_time);

    for(size_t ithres=0; ithres<ThresholdData::nthres; ithres++){
        fd_.tree_->SetBranchAddress(("raise_time_th" + std::to_string(ThresholdData::thresholds[ithres])).c_str(), &fd_.ef_.raise_times[ithres]);
        fd_.tree_->SetBranchAddress(("fall_time_th" + std::to_string(ThresholdData::thresholds[ithres])).c_str(), &fd_.ef_.fall_times[ithres]);
        fd_.tree_->SetBranchAddress(("charge_th" + std::to_string(ThresholdData::thresholds[ithres])).c_str(), &fd_.ef_.charges[ithres]);
        fd_.tree_->SetBranchAddress(("tot_th" + std::to_string(ThresholdData::thresholds[ithres])).c_str(), &fd_.ef_.tots[ithres]);
    }

    fd_.nentries_ = fd_.tree_->GetEntries();
}

FeatureReader::~FeatureReader(){
    if(fd_.file_){
        fd_.file_->Close();
        delete fd_.file_;
    }
}