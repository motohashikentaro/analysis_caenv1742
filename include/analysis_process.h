#ifndef ANALYSIS_PROCESS_H
#define ANALYSIS_PROCESS_H

#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>

#include "./rootfile_analyzer.h"

class AnalysisProcess{
    public:
        AnalysisProcess(RootData& rd): rd_(rd){};  // Constructor to initialize RootData reference
        ~AnalysisProcess(){};  // Destructor

        void MinAdcDistro();
        void Multiplicity();
        void HitMap();
    private:
        RootData& rd_;
};

#endif