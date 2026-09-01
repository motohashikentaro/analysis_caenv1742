#ifndef THROUGH_EVENT_READER_H
#define THROUGH_EVENT_READER_H

#include "./../include/through_event_extractor.h"

#include <string>

#include <TFile.h>
#include <TTree.h>

struct ThroughEventData{
    ThroughEvent te_;

    TFile* file_ = nullptr;
    std::string filename_;
    std::string run_number_;
    std::string feature_condition_;
    std::string tracker_condition_;
    std::string dut_condition_;
    std::string strip_reconstruction_condition_;
    std::string pixel_reconstruction_condition_;
    TTree* tree_ = nullptr;
    Long64_t nentries_ = 0;

    std::string HitCondition() const{
        return tracker_condition_ + "-" + dut_condition_;
    }
    std::string ReconstructionCondition() const{
        return strip_reconstruction_condition_ + "-" + pixel_reconstruction_condition_;
    }
};

class ThroughEventReader{
    public:
        ThroughEventReader(const char* input_path);  // Constructor to initialize ThroughEventData from the input ROOT file
        ~ThroughEventReader();  // Destructor

        ThroughEventData& GetThroughEventData(){return td_;}  // Accessor to ThroughEventData
    private:
        ThroughEventData td_;
};

#endif