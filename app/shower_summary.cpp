#include "./../include/correlation_analyzer.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/alignment_data.h"

#include <array>
#include <iostream>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TColor.h>
#include <TF1.h>
#include <TLine.h>

int main(int argc, char** argv){

    ThroughEventReader tr(argv[1]);
    CorrelationAnalyzer ca(tr.GetThroughEventData());

    TH2D* hist1 = ca.FrontStripBackStripXCorrelation();
    TH2D* hist2 = ca.FrontStripBackStripYCorrelation();
    TH2D* hist3 = ca.FrontStripHitmap();
    TH2D* hist4 = ca.BackStripHitmap();
    TH2D* hist5 = ca.FrontDutHitmap();
    TH2D* hist6 = ca.BackDutHitmap();
    TH2D* hist19 = ca.FrontExtrapolatedTrackHitmap();
    TH2D* hist20 = ca.BackExtrapolatedTrackHitmap();
    TH2D* hist21 = ca.FrontDutPositionExtrapolatedTrackXCorrelation();
    TH2D* hist22 = ca.BackDutPositionExtrapolatedTrackXCorrelation();
    TH2D* hist23 = ca.FrontDutPositionExtrapolatedTrackYCorrelation();
    TH2D* hist24 = ca.BackDutPositionExtrapolatedTrackYCorrelation();
    TH1D* hist25 = ca.DifferenceFrontExtrapolatedTrackDutPositionXWithShower();
    TH1D* hist26 = ca.DifferenceBackExtrapolatedTrackDutPositionXWithShower();
    TH1D* hist27 = ca.DifferenceFrontExtrapolatedTrackDutPositionYWithShower();
    TH1D* hist28 = ca.DifferenceBackExtrapolatedTrackDutPositionYWithShower();

    gStyle->SetOptStat(0);
    auto pramary_color = TColor::GetColor("#008899");
    TLatex* latex = new TLatex();
    latex->SetNDC();

    AlignmentData alignment_data;

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
        // if(i == 2){
        //     hist_array2[i]->GetYaxis()->SetRangeUser(-2 - alignment_data.front_y, 2 - alignment_data.front_y);
        //     hist_array2[i]->GetXaxis()->SetRangeUser(-2 - alignment_data.front_x, 2 - alignment_data.front_x);
        // }
        // if(i == 3){
        //     hist_array2[i]->GetYaxis()->SetRangeUser(-2 - alignment_data.back_y, 2 - alignment_data.back_y);
        //     hist_array2[i]->GetXaxis()->SetRangeUser(-2 - alignment_data.back_x, 2 - alignment_data.back_x);
        // }
        hist_array2[i]->Draw("COLZ");
        if(i == 2){
            const double xmin = -1.0 - alignment_data.front_x;
            const double xmax =  1.0 - alignment_data.front_x;
            const double ymin = -1.0 - alignment_data.front_y;
            const double ymax =  1.0 - alignment_data.front_y;

            const double boundaries[] = {-0.5, 0.0, 0.5};

            for(double x : boundaries){
                // 本来のpixel境界
                const double boundary_x = x - alignment_data.front_x;

                // その境界が入っているbinの中心へ線を移動
                const int bin = hist_array2[i]->GetXaxis()->FindBin(boundary_x);
                const double line_x = hist_array2[i]->GetXaxis()->GetBinCenter(bin);

                TLine* line = new TLine(
                    line_x, ymin,
                    line_x, ymax
                );

                line->SetLineColor(kRed);
                line->SetLineWidth(2);
                line->Draw();
            }

            for(double y : boundaries){
                const double boundary_y = y - alignment_data.front_y;

                const int bin = hist_array2[i]->GetYaxis()->FindBin(boundary_y);
                const double line_y = hist_array2[i]->GetYaxis()->GetBinCenter(bin);

                TLine* line = new TLine(
                    xmin, line_y,
                    xmax, line_y
                );

                line->SetLineColor(kRed);
                line->SetLineWidth(2);
                line->Draw();
            }
        }
        if(i == 3){
            TLine* line1 = new TLine(-0.25 - alignment_data.back_x, -1 - alignment_data.back_y, -0.25 - alignment_data.back_x, 1 - alignment_data.back_y);
            TLine* line2 = new TLine(0.25 - alignment_data.back_x, -1 - alignment_data.back_y, 0.25 - alignment_data.back_x, 1 - alignment_data.back_y);
            TLine* line3 = new TLine(-0.75 - alignment_data.back_x, -1 - alignment_data.back_y, -0.75 - alignment_data.back_x, 1 - alignment_data.back_y);
            TLine* line4 = new TLine(0.75 - alignment_data.back_x, -1 - alignment_data.back_y, 0.75 - alignment_data.back_x, 1 - alignment_data.back_y);
            TLine* line5 = new TLine(-1 - alignment_data.back_x, -0.25 - alignment_data.back_y, 1 - alignment_data.back_x, -0.25 - alignment_data.back_y);
            TLine* line6 = new TLine(-1 - alignment_data.back_x, 0.25 - alignment_data.back_y, 1 - alignment_data.back_x, 0.25 - alignment_data.back_y);
            TLine* line7 = new TLine(-1 - alignment_data.back_x, -0.75 - alignment_data.back_y, 1 - alignment_data.back_x, -0.75 - alignment_data.back_y);
            TLine* line8 = new TLine(-1 - alignment_data.back_x, 0.75 - alignment_data.back_y, 1 - alignment_data.back_x, 0.75 - alignment_data.back_y);
            line1->SetLineColor(kRed);
            line2->SetLineColor(kRed);
            line3->SetLineColor(kRed);
            line4->SetLineColor(kRed);
            line5->SetLineColor(kRed);
            line6->SetLineColor(kRed);
            line7->SetLineColor(kRed);
            line8->SetLineColor(kRed);
            line1->SetLineWidth(2);
            line2->SetLineWidth(2);
            line3->SetLineWidth(2);
            line4->SetLineWidth(2);
            line5->SetLineWidth(2);
            line6->SetLineWidth(2);
            line7->SetLineWidth(2);
            line8->SetLineWidth(2);
            line1->Draw();
            line2->Draw();
            line3->Draw();
            line4->Draw();
            line5->Draw();
            line6->Draw();
            line7->Draw();
            line8->Draw();
        }
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
        // if(i == 0) hist_array7[i]->GetYaxis()->SetRangeUser(-2 - alignment_data.front_x, 2 - alignment_data.front_x);
        // if(i == 1) hist_array7[i]->GetYaxis()->SetRangeUser(-2 - alignment_data.back_x, 2 - alignment_data.back_x);
        // if(i == 2) hist_array7[i]->GetYaxis()->SetRangeUser(-2 - alignment_data.front_y, 2 - alignment_data.front_y);
        // if(i == 3) hist_array7[i]->GetYaxis()->SetRangeUser(-2 - alignment_data.back_y, 2 - alignment_data.back_y);
        hist_array7[i]->Draw("COLZ");
    }

auto* canvas8 = PlotSupporter::MakeCanvas1D("canvas8", 2, 2);
std::array<TH1D*, 4> hist_array8 = {hist25, hist26, hist27, hist28};

// histogramごとのGaussian fit範囲
std::array<std::pair<double, double>, 4> fit_ranges = {{
    {0, 0.6},   // hist25
    {-0.3, 0.8},   // hist26
    {-0.3, 0.3},   // hist27
    {-0.4, 0.8}    // hist28
}};

std::array<const char*, 4> x_titles = {{
    "x_{DUT}^{Front} - x_{Extrapolated}^{Front} [mm]",
    "x_{DUT}^{Back} - x_{Extrapolated}^{Back} [mm]",
    "y_{DUT}^{Front} - y_{Extrapolated}^{Front} [mm]",
    "y_{DUT}^{Back} - y_{Extrapolated}^{Back} [mm]"
}};


for(int i=0; i<4; ++i){

    auto* pad = canvas8->cd(i+1);

    PlotSupporter::SetHistStyle1D(hist_array8[i]);

    // padの余白
    pad->SetLeftMargin(0.15);
    pad->SetBottomMargin(0.15);
    pad->SetRightMargin(0.05);
    pad->SetTopMargin(0.05);

    // 軸タイトル
    hist_array8[i]->GetXaxis()->SetTitle(x_titles[i]);
    hist_array8[i]->GetXaxis()->SetTitleSize(0.045);
    hist_array8[i]->GetXaxis()->SetTitleOffset(1.15);

    hist_array8[i]->GetYaxis()->SetTitleSize(0.045);
    hist_array8[i]->GetYaxis()->SetTitleOffset(1.35);

    // histogramごとのfit範囲
    const auto [fit_min, fit_max] = fit_ranges[i];

    hist_array8[i]->Fit(
        "gaus",
        "0Q",
        "",
        fit_min,
        fit_max
    );

    hist_array8[i]->Draw("HIST");

    TF1* fit = hist_array8[i]->GetFunction("gaus");

    if(fit){
        const double mean  = fit->GetParameter(1);
        const double sigma = fit->GetParameter(2);

        fit->Draw("SAME");

        latex->DrawLatexNDC(
            0.62, 0.80,
            Form("#mu = %.3f", mean)
        );

        latex->DrawLatexNDC(
            0.62, 0.70,
            Form("#sigma = %.3f", sigma)
        );

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
    canvas6->SaveAs((save_dir / "extrapolated_track_hitmap.png").c_str());
    canvas7->SaveAs((save_dir / "dut_extrapolated_track_correlation.png").c_str());
    canvas8->SaveAs((save_dir / "difference_extrapolated_track_dut_position.png").c_str());
    return 0;
}