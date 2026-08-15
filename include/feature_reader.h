#ifndef FEATURE_READER_H
#define FEATURE_READER_H

#include "feature_extractor.h"

#include <string>

#include <TFile.h>
#include <TTree.h>

struct FeatureData{
    EvtFeature ef_;

    TFile* file_;
    std::string filename_;
    std::string run_number_;
    TTree* tree_;
    Long64_t nentries_;
};

class FeatureReader{
    public:
        FeatureReader(char* input_path);
        ~FeatureReader();

        FeatureData& GetFeatureData(){return fd_;}

    private:
        FeatureData fd_;
};

#endif