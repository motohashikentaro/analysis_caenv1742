#ifndef FEATURE_READER_H
#define FEATURE_READER_H

#include "feature_extractor.h"

#include <string>

#include <TFile.h>
#include <TTree.h>

struct FeatureData{
    EvtFeature ef_;

    TFile* file_ = nullptr;
    std::string filename_;
    std::string run_number_;
    std::string feature_condition_;
    TTree* tree_ = nullptr;
    Long64_t nentries_ = 0;
};

class FeatureReader{
    public:
        FeatureReader(const char* input_path);
        ~FeatureReader();

        FeatureData& GetFeatureData(){return fd_;}

    private:
        FeatureData fd_;
};

#endif