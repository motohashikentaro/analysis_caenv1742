#ifndef ROOTFILE_READER_H
#define ROOTFILE_READER_H

#include <string>

#include <TFile.h>
#include <TTree.h>

struct Event{
    Long64_t ev_id;
    float amp[2][32][1024];
};

struct RootData{
    Event ev_;
    
    int nboard = 2;  // digitizer board
    int nch = 32;
    int nsample = 1024;

    TFile* file_;
    std::string filename_;
    std::string run_number_;
    TTree* tree_;
    Long64_t nentries_;
};

class RootfileReader{
    public:
        RootfileReader(char* input_path);  // Constructor to initialize RootData from the input ROOT file
        ~RootfileReader();  // Destructor

        RootData& GetRootData(){return rd_;}  // Accessor to RootData
    private:
        RootData rd_;
};

#endif