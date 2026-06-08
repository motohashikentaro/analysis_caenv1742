#ifndef ROOTFILE_ANALYZER_H
#define ROOTFILE_ANALYZER_H

#include <TFile.h>
#include <TTree.h>

struct event{
    Long64_t ev_id;
    UShort_t amp[2][32][1024];
    
};

class RootFileAnalyzer{
    public:
         
    private:
};

#endif