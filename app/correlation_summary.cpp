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

    const int cut_n_hit_ch_front = std::stoi(argv[2]);
    const int cut_n_hit_ch_back = std::stoi(argv[3]);

    HitReader hr(argv[1]);
    CorrelationAnalyzer ca(hr.GetHitData());

    TH2D* hist1 = ca.FrontStripBackStripXCorrelation();
    TH2D* hist2 = ca.FrontStripBackStripYCorrelation();
    TH2D* hist3 = ca.FrontStripHitmap();
    TH2D* hist4 = ca.BackStripHitmap();
    TH2D* hist5 = ca.FrontDutHitmap();
    TH2D* hist6 = ca.BackDutHitmap();
    TH2D* hist7 = ca.FrontStripFrontDutXCorrelation(cut_n_hit_ch_front);
    TH2D* hist8 = ca.FrontStripBackDutXCorrelation(cut_n_hit_ch_back);
    TH2D* hist9 = ca.BackStripFrontDutXCorrelation(cut_n_hit_ch_front);
    TH2D* hist10 = ca.BackStripBackDutXCorrelation(cut_n_hit_ch_back);
    TH2D* hist11 = ca.FrontStripFrontDutYCorrelation(cut_n_hit_ch_front);
    TH2D* hist12 = ca.FrontStripBackDutYCorrelation(cut_n_hit_ch_back);
    TH2D* hist13 = ca.BackStripFrontDutYCorrelation(cut_n_hit_ch_front);
    TH2D* hist14 = ca.BackStripBackDutYCorrelation(cut_n_hit_ch_back);
    TH2D* hist15 = ca.FrontExtrapolatedTrackDutPositionXCorrelation(cut_n_hit_ch_front);
    TH2D* hist16 = ca.BackExtrapolatedTrackDutPositionXCorrelation(cut_n_hit_ch_back);
    TH2D* hist17 = ca.FrontExtrapolatedTrackDutPositionYCorrelation(cut_n_hit_ch_front);
    TH2D* hist18 = ca.BackExtrapolatedTrackDutPositionYCorrelation(cut_n_hit_ch_back);
    TH2D* hist19 = ca.FrontExtrapolatedTrackHitmap();
    TH2D* hist20 = ca.BackExtrapolatedTrackHitmap();

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

    auto* canvas4 = PlotSupporter::MakeCanvas2D("canvas4", 2, 2);
    std::array<TH2D*, 4> hist_array4 = {hist11, hist12, hist13, hist14};
    for(int i=0; i<4; ++i){
        canvas4->cd(i+1);
        PlotSupporter::SetHistStyle2D(hist_array4[i]);
        hist_array4[i]->Draw("COLZ");
    }

    auto* canvas5 = PlotSupporter::MakeCanvas2D("canvas5", 2, 2);
    std::array<TH2D*, 4> hist_array5 = {hist15, hist16, hist17, hist18};
    for(int i=0; i<4; ++i){
        canvas5->cd(i+1);
        PlotSupporter::SetHistStyle2D(hist_array5[i]);
        hist_array5[i]->Draw("COLZ");
    }

    auto* canvas6 = PlotSupporter::MakeCanvas2D("canvas6", 2, 1);
    std::array<TH2D*, 2> hist_array6 = {hist19, hist20};
    for(int i=0; i<2; ++i){
        canvas6->cd(i+1);
        PlotSupporter::SetHistStyle2D(hist_array6[i]);
        hist_array6[i]->Draw("COLZ");
    }

    canvas1->SaveAs(("./../result/strip_correlation_" + hr.GetHitData().run_number_condition_ + ".png").c_str());
    canvas2->SaveAs(("./../result/strip_dut_hitmap_" + hr.GetHitData().run_number_condition_ + ".png").c_str());
    canvas3->SaveAs(("./../result/strip_dut_xcorrelation_" + hr.GetHitData().run_number_condition_ + ".png").c_str());
    canvas4->SaveAs(("./../result/strip_dut_ycorrelation_" + hr.GetHitData().run_number_condition_ + ".png").c_str());
    canvas5->SaveAs(("./../result/extrapolated_track_dut_correlation_" + hr.GetHitData().run_number_condition_ + ".png").c_str());
    canvas6->SaveAs(("./../result/extrapolated_track_hitmap_" + hr.GetHitData().run_number_condition_ + ".png").c_str());
    return 0;
}