#ifndef HIT_READER_H
#define HIT_READER_H

#include "hit_extractor.h"

#include <TFile.h>
#include <TTree.h>

struct HitData{
    EvtHit eh_;

    TFile* file_;
    std::string filename_;
    std::string run_number_condition_;
    TTree* tree_;
    Long64_t nentries_;
};

class HitReader{
    public:
        HitReader(char* input_path);
        ~HitReader();

        HitData& GetHitData(){return hd_;}

    private:
        HitData hd_;
};

#endif