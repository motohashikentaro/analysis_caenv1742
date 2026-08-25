#include "./../include/correlation_analyzer.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/hit_analyzer.h"

#include <array>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TColor.h>



int main(int argc, char** argv){

    TH1D* hist1 = new TH1D("front", ";Front n hit ch;Entries", 16, 0, 16);
    TH1D* hist2 = new TH1D("back", ";Back n hit ch;Entries", 16, 0, 16);

    HitReader hr(argv[1]);
    MatchedHitAnalyzer mha(hr.GetHitData());
    
    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist1->Fill(rhp.n_hit_ch_front);
        hist2->Fill(rhp.n_hit_ch_back);
    });

    gStyle->SetOptStat(0);
    auto pramary_color = TColor::GetColor("#008899");
    
    auto* canvas1 = PlotSupporter::MakeCanvas1D("canvas1", 2, 1);
    std::array<TH1D*, 2> hist_array1 = {hist1, hist2};
    for(int i=0; i<2; ++i){
        canvas1->cd(i+1);
        PlotSupporter::SetHistStyle1D(hist_array1[i]);
        hist_array1[i]->Draw("HIST");
    }

    canvas1->SaveAs(("./../result/n_hit_ch_" + hr.GetHitData().run_number_condition_ + ".png").c_str());

}