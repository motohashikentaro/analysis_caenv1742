#ifndef HIT_ANALYZER_H
#define HIT_ANALYZER_H

#include "./../include/hit_reader.h"

#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TGraph.h>

class HitAnalyzer{
    public:
        HitAnalyzer(HitData& hd): hd_(hd){};  // Constructor to initialize HitData reference

        TH1D* PeakDistro(int board, int ch);
        TH1D* PeaktimeDistro(int board, int ch);
        TH1D* ChargeDistro(int board, int ch);
        TH2D* PeakVsCharge(int board, int ch);
        TH1D* TotDistro(int board, int ch);
        TH2D* PeakVsTot(int board, int ch);


    private:
        HitData& hd_;
};

#endif