// #ifndef HIT_ANALYZER_H
// #define HIT_ANALYZER_H

// #include "./hit_selection.h"

// #include <array>
// #include <string>

// #include <TFile.h>
// #include <TTree.h>
// #include <TH1D.h>
// #include <TH2D.h>

// struct HitData{
//     TFile* file_ = nullptr;

//     std::string filename_;
//     std::string run_number_;

//     std::array<TTree*, nthres> trees_{};
//     std::array<Long64_t, nthres> nentries_{};

//     EvtFeature ef_;
// };

// class HitAnalyzer{
// public:
//     HitAnalyzer(char* input_path);
//     ~HitAnalyzer();

//     TH1D* PeakDistro();
//     std::array<TH1D*, nthres> ChargeDistro();
//     TH1D* PeaktimeDistro();
//     std::array<TH2D*, nthres> PeakVsCharge();
//     TH1D* TotDistro();

//     const HitData& GetHitData() const{
//         return hd_;
//     }

// private:
//     HitData hd_;
// };

// #endif

#ifndef HIT_ANALYZER_H
#define HIT_ANALYZER_H

#include "./hit_selection.h"

#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>


struct HitData{
    TFile* file_ = nullptr;

    std::string filename_;
    std::string run_number_;

    TTree* tree_ = nullptr;
    Long64_t nentries_ = 0;

    EvtFeature ef_;
};


class HitAnalyzer{
public:
    HitAnalyzer(char* input_path);
    ~HitAnalyzer();


    TH1D* PeakDistro(int ch);

    TH1D* ChargeDistro(int ch);

    TH1D* PeaktimeDistro(int ch);

    TH2D* PeakVsCharge(int ch);

    TH1D* TotDistro(int ch);


    void DrawHistInfo(TH1* hist,
                      int ch,
                      double x = 0.55,
                      double y = 0.85);


    const HitData& GetHitData() const{
        return hd_;
    }


private:
    HitData hd_;
};


#endif