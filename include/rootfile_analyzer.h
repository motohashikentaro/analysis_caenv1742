#ifndef ROOTFILE_ANALYZER_H
#define ROOTFILE_ANALYZER_H

#include "./rootfile_reader.h"

#include <TGraph.h>

class RootfileAnalyzer{
    public:
        RootfileAnalyzer(RootData& rd): rd_(rd){};  // Constructor to initialize RootData reference

        std::vector<TGraph*> Waveform(int target_board, int target_ch, const std::vector<Long64_t>& target_evt, int range_start=0, int range_end=1024);  // Method to get waveforms for a specific board and channel

    private:
        RootData& rd_;
};

#endif