#ifndef HIT_READER_H
#define HIT_READER_H

#include "hit_extractor.h"

#include <TFile.h>
#include <TTree.h>

struct HitData{
    EvtHit eh_;

    TFile* file_ = nullptr;
    std::string filename_;
    std::string run_number_;
    std::string feature_condition_;
    std::string tracker_condition_;
    std::string dut_condition_;
    TTree* tree_ = nullptr;
    Long64_t nentries_ = 0;

    std::string HitCondition() const{
        return tracker_condition_ + "-" + dut_condition_;
    }
};

class HitReader{
    public:
        HitReader(const char* input_path);
        ~HitReader();

        HitData& GetHitData(){return hd_;}

    private:
        HitData hd_;
};

#endif