#include "./../include/correlation_analyzer.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"

#include <array>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TColor.h>

int main(int argc, char** argv){

    HitReader hr(argv[1]);
    CorrelationAnalyzer ca(hr.GetHitData());

    TH2D* hist1 = ca.StripXCorrelation();
    TH2D* hist2 = ca.StripYCorrelation();
    TH2D* hist3 = ca.FrontStripHitmap();
    TH2D* hist4 = ca.BackStripHitmap();
    TH2D* hist5 = ca.FrontDutHitmap();
    TH2D* hist6 = ca.BackDutHitmap();
    TH2D* hist7 = ca.FrontStripFrontDutCorrelation();
    TH2D* hist8 = ca.FrontStripBackDutCorrelation();
    TH2D* hist9 = ca.BackStripFrontDutCorrelation();
    TH2D* hist10 = ca.BackStripBackDutCorrelation();

    gStyle->SetOptStat(0);
    auto pramary_color = TColor::GetColor("#008899");

    // 2D histogram
    auto* canvas1 = PlotSupporter::MakeCanvas2D("canvas1", 2, 1);
    std::array<TH2D*, 2> hist_array1 = {hist1, hist2};
    for(int i=0; i<2; ++i){
        canvas1->cd(i+1);
        PlotSupporter::SetHistStyle2D(hist_array1[i]);
        hist_array1[i]->Draw("COLZ");
    }

    auto* canvas2 = PlotSupporter::MakeCanvas2D("canvas2", 2, 2);
    std::array<TH2D*, 4> hist_array2 = {hist3, hist4, hist5, hist6};
    for(int i=0; i<4; ++i){
        canvas2->cd(i+1);
        PlotSupporter::SetHistStyle2D(hist_array2[i]);
        hist_array2[i]->Draw("COLZ");
    }

    auto* canvas3 = PlotSupporter::MakeCanvas2D("canvas3", 2, 2);
    std::array<TH2D*, 4> hist_array3 = {hist7, hist8, hist9, hist10};
    for(int i=0; i<4; ++i){
        canvas3->cd(i+1);
        PlotSupporter::SetHistStyle2D(hist_array3[i]);
        hist_array3[i]->Draw("COLZ");
    }

    canvas1->SaveAs(("./../result/strip_correlation_" + hr.GetHitData().run_number_condition_ + ".png").c_str());
    canvas2->SaveAs(("./../result/strip_dut_hitmap_" + hr.GetHitData().run_number_condition_ + ".png").c_str());
    canvas3->SaveAs(("./../result/strip_dut_correlation_" + hr.GetHitData().run_number_condition_ + ".png").c_str());

    return 0;
}