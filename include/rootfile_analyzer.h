#ifndef ROOTFILE_ANALYZER_H
#define ROOTFILE_ANALYZER_H

#include <TFile.h>
#include <TTree.h>

struct event{
    Long64_t ev_id;
    UShort_t amp[2][32][1024];
};

struct RootData{
    event ev_;
    
    int nboard = 2;
    int nch = 32;
    int nsample = 1024;

    TFile* file_;
    TTree* tree_;
    Long64_t nentries_;
};

class RootfileAnalyzer{
    public:
        RootfileAnalyzer(char* input_path);  // Constructor to initialize RootData from the input ROOT file
        ~RootfileAnalyzer();  // Destructor

        RootData& GetRootData(){return rd_;}  // Accessor to RootData

        void EventLoop();
    private:
        RootData rd_;
};

#endif