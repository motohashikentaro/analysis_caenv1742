#include "./../include/correlation_analyzer.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"

#include <array>
#include <iostream>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TColor.h>
#include <TF1.h>

int main(int argc, char** argv){

    ThroughEventReader tr(argv[1]);
    CorrelationAnalyzer ca(tr.GetThroughEventData());

    TH2D* hist1 = ca.FrontStripBackStripXCorrelation();
    TH2D* hist2 = ca.FrontStripBackStripYCorrelation();
    TH2D* hist3 = ca.FrontStripHitmap();
    TH2D* hist4 = ca.BackStripHitmap();
    TH2D* hist5 = ca.FrontDutHitmap();
    TH2D* hist6 = ca.BackDutHitmap();
    TH2D* hist7 = ca.FrontStripFrontDutXCorrelation();
    TH2D* hist8 = ca.FrontStripBackDutXCorrelation();
    TH2D* hist9 = ca.BackStripFrontDutXCorrelation();
    TH2D* hist10 = ca.BackStripBackDutXCorrelation();
    TH2D* hist11 = ca.FrontStripFrontDutYCorrelation();
    TH2D* hist12 = ca.FrontStripBackDutYCorrelation();
    TH2D* hist13 = ca.BackStripFrontDutYCorrelation();
    TH2D* hist14 = ca.BackStripBackDutYCorrelation();
    TH2D* hist19 = ca.FrontExtrapolatedTrackHitmap();
    TH2D* hist20 = ca.BackExtrapolatedTrackHitmap();
    TH2D* hist21 = ca.FrontDutPositionExtrapolatedTrackXCorrelation();
    TH2D* hist22 = ca.BackDutPositionExtrapolatedTrackXCorrelation();
    TH2D* hist23 = ca.FrontDutPositionExtrapolatedTrackYCorrelation();
    TH2D* hist24 = ca.BackDutPositionExtrapolatedTrackYCorrelation();
    TH1D* hist25 = ca.DifferenceFrontExtrapolatedTrackDutPositionX();
    TH1D* hist26 = ca.DifferenceBackExtrapolatedTrackDutPositionX();
    TH1D* hist27 = ca.DifferenceFrontExtrapolatedTrackDutPositionY();
    TH1D* hist28 = ca.DifferenceBackExtrapolatedTrackDutPositionY();

    gStyle->SetOptStat(0);
    auto pramary_color = TColor::GetColor("#008899");
    TLatex* latex = new TLatex();
    latex->SetNDC();

    PlotSupporter ps(tr.GetThroughEventData());
    std::filesystem::path save_dir = ps.MakeSaveDir();

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

    auto* canvas6 = PlotSupporter::MakeCanvas2D("canvas6", 2, 1);
    std::array<TH2D*, 2> hist_array6 = {hist19, hist20};
    for(int i=0; i<2; ++i){
        canvas6->cd(i+1);
        PlotSupporter::SetHistStyle2D(hist_array6[i]);
        hist_array6[i]->Draw("COLZ");
    }

    auto* canvas7 = PlotSupporter::MakeCanvas2D("canvas7", 2, 2);
    std::array<TH2D*, 4> hist_array7 = {hist21, hist22, hist23, hist24};
    for(int i=0; i<4; ++i){
        canvas7->cd(i+1);
        PlotSupporter::SetHistStyle2D(hist_array7[i]);
        hist_array7[i]->Draw("COLZ");
    }

    auto* canvas8 = PlotSupporter::MakeCanvas1D("canvas8", 2, 2);
    std::array<TH1D*, 4> hist_array8 = {hist25, hist26, hist27, hist28};

    std::array<std::pair<double, double>, 4> fit_ranges = {{
        {-1.5,  -0.8},   // hist25
        {-1,  0},   // hist26
        {-1.3,  -0.7},   // hist27
        {-1.8,  -1.1}    // hist28
    }};

    for(int i=0; i<4; ++i){
        canvas8->cd(i+1);

        PlotSupporter::SetHistStyle1D(hist_array8[i]);

        const auto [xmin, xmax] = fit_ranges[i];

        hist_array8[i]->Fit("gaus", "0Q", "", xmin, xmax);
        hist_array8[i]->Draw("HIST");

        TF1* fit = hist_array8[i]->GetFunction("gaus");

        if(fit){
            const double mean = fit->GetParameter(1);
            const double sigma = fit->GetParameter(2);

            fit->Draw("SAME");

            latex->DrawLatexNDC(0.6, 0.8, Form("#mu = %.3f", mean));
            latex->DrawLatexNDC(0.6, 0.7, Form("#sigma = %.3f", sigma));

            std::cout
                << hist_array8[i]->GetName()
                << " mean: " << mean
                << ", sigma: " << sigma
                << std::endl;
        }
    }

    TFile* output_file = new TFile((save_dir / "correlation_summary.root").c_str(), "RECREATE");
    output_file->cd();
    hist21->Write();
    hist22->Write();
    hist23->Write();
    hist24->Write();
    hist25->Write();
    hist26->Write();
    hist27->Write();
    hist28->Write();
    output_file->Close();

    canvas1->SaveAs((save_dir / "strip_correlation.png").c_str());
    canvas2->SaveAs((save_dir / "strip_dut_hitmap.png").c_str());
    canvas3->SaveAs((save_dir / "strip_dut_xcorrelation.png").c_str());
    canvas4->SaveAs((save_dir / "strip_dut_ycorrelation.png").c_str());
    canvas6->SaveAs((save_dir / "extrapolated_track_hitmap.png").c_str());
    canvas7->SaveAs((save_dir / "dut_extrapolated_track_correlation.png").c_str());
    canvas8->SaveAs((save_dir / "difference_extrapolated_track_dut_position.png").c_str());
    return 0;
}