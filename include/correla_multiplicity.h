#ifndef CORRELA_MULTIPLICITY_H
#define CORRELA_MULTIPLICITY_H

#include "hit_correlation.h"

#include <array>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TGraph.h>

class CorrelaMultiplicity{

public:

    CorrelaMultiplicity(
        const std::string& filename
    );

    ~CorrelaMultiplicity();


    void Analyze();

    void Draw();

    void Save(
        const std::string& outdir
    );


private:

    static constexpr int nenergy = 5;

    static constexpr int nnw = 6;


    static constexpr std::array<int, nnw> w_values = {
        0, 2, 3, 4, 5, 6
    };


    //==============================
    // input
    //==============================

    TFile* file_;

    TTree* tree_;


    EventHit eh_;

    int energy_;

    int nw_;


    //==============================
    // Energy dependence (W = 2)
    //==============================

    // all events

    std::array<TH1D*, nenergy>
        h_front_energy_all_;

    std::array<TH1D*, nenergy>
        h_back_energy_all_;


    // shower events

    std::array<TH1D*, nenergy>
        h_front_energy_sh_;

    std::array<TH1D*, nenergy>
        h_back_energy_sh_;


    //==============================
    // W dependence (Energy = 2)
    //==============================

    // all events

    std::array<TH1D*, nnw>
        h_front_nw_all_;

    std::array<TH1D*, nnw>
        h_back_nw_all_;


    // shower events

    std::array<TH1D*, nnw>
        h_front_nw_sh_;

    std::array<TH1D*, nnw>
        h_back_nw_sh_;


    //==============================
    // internal functions
    //==============================

    void SetBranchAddresses();

    void CreateHistograms();


    void DrawEnergy(
        const std::array<TH1D*, nenergy>& hists,
        const std::string& title,
        const std::string& outfile
    );


    void DrawNW(
        const std::array<TH1D*, nnw>& hists,
        const std::string& title,
        const std::string& outfile
    );


    void DrawEnergySummary(
        const std::array<TH1D*, nenergy>& front_hists,
        const std::array<TH1D*, nenergy>& back_hists,
        const std::string& outdir
    );


    void DrawNWSummary(
        const std::array<TH1D*, nnw>& front_hists,
        const std::array<TH1D*, nnw>& back_hists,
        const std::string& outdir
    );

};

#endif