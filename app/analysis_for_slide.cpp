#include "./../include/feature_analyzer.h"
#include "./../include/feature_reader.h"
#include "./../include/plot_supporter.h"

#include <iostream>
#include <array>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TColor.h>
#include <TF1.h>
#include <TLine.h>



int main(int argc, char* argv[]){
    if(argc != 5 && argc != 6){
        std::cerr << "Usage: " << argv[0] << " <input_root_file> <board> <ch> <target_threshold> [<cut_line>]" << std::endl;
        return 1;
    }
    FeatureReader fr(argv[1]);
    FeatureAnalyzer fa(fr.GetFeatureData());

    int board = std::stoi(argv[2]);
    int ch = std::stoi(argv[3]);
    int target_threshold = std::stoi(argv[4]);

    TH1D* hist1 = fa.PeakDistro(board, ch);
    TH1D* hist2 = fa.PeaktimeDistro(board, ch);
    TH1D* hist3 = fa.ChargeDistro(board, ch, target_threshold);
    TH2D* hist4 = fa.PeakVsCharge(board, ch, target_threshold);
    TH1D* hist5 = fa.TotDistro(board, ch, target_threshold);
    TH2D* hist6 = fa.PeakVsTot(board, ch, target_threshold);
    TH1D* hist7 = fa.ChargePerPeakDistro(board, ch, target_threshold);
    TH1D* hist8 = fa.TotPerPeakDistro(board, ch, target_threshold);
    TH1D* hist9 = fa.ChargeMinusAlphaAdcDistro(board, ch, target_threshold);
    TH1D* hist10 = fa.TotMinusAlphaAdcDistro(board, ch, target_threshold);
    TH1D* hist11 = fa.SlopeDistro(board, ch, target_threshold);
    TH2D* hist12 = fa.TotVsSlope(board, ch, target_threshold);
    TH2D* hist13 = fa.PeakVsSlope(board, ch, target_threshold);
    TH2D* hist14 = fa.ChargeVsSlope(board, ch, target_threshold);
    TH2D* hist15 = fa.PeaktimeVsSlope(board, ch, target_threshold);
    TH2D* hist16 = fa.PeakVsPeaktime(board, ch, target_threshold);

    gStyle->SetOptStat(0);
    auto pramary_color = TColor::GetColor("#008899");
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);

    PlotSupporter ps(fr.GetFeatureData());
    std::filesystem::path save_dir = ps.MakeSaveDir();

    // 1D histogram
    auto* canvas = PlotSupporter::MakeCanvas1D("canvas", 4, 1);
    std::array<TH1*, 4> histograms = {hist1, hist2, hist3, hist5};
    for(size_t i = 0; i < histograms.size(); ++i){
        canvas->cd(i + 1);
        PlotSupporter::SetHistStyle1D(histograms[i]);
        if(i == 2) histograms[i]->GetXaxis()->SetRangeUser(1, 1000);
        if(i == 3) histograms[i]->GetXaxis()->SetRangeUser(1, 15);
        histograms[i]->Draw("HIST");
        latex->DrawLatex(0.50, 0.80, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.50, 0.77, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.50, 0.74, Form("Threshold %d ADC", target_threshold));
    }

    canvas->Update();
    std::string filename = "feature_summary_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_threshold_" + std::to_string(target_threshold) + ".png";
    canvas->SaveAs((save_dir / filename).string().c_str());

    // 2D histogram
    auto* canvas2d = PlotSupporter::MakeCanvas2D("canvas2d", 2, 1);
    std::array<TH2D*, 2> histograms2d = {hist4, hist6};
    for(size_t i = 0; i < histograms2d.size(); ++i){
        canvas2d->cd(i + 1);
        PlotSupporter::SetHistStyle2D(histograms2d[i]);
        if(i == 0){
            histograms2d[i]->GetXaxis()->SetRangeUser(0, 400);
            histograms2d[i]->GetYaxis()->SetRangeUser(0, 1000);
        }
        if(i == 1){
            histograms2d[i]->GetXaxis()->SetRangeUser(0, 400);
            histograms2d[i]->GetYaxis()->SetRangeUser(0, 15);
        }
        histograms2d[i]->Draw("COLZ");

        if(i == 0){
            if(argc > 5 && std::string(argv[5]) == "line_combined"){
                TLine* cut_line1 = new TLine(20, 0, 20, 1000);
                cut_line1->SetLineColor(kRed);
                cut_line1->SetLineWidth(2);
                cut_line1->Draw("SAME");
                TF1* cut_line2 = new TF1("cut_line2", "2.5*x-60", 30, 80);
                cut_line2->SetLineColor(kRed);
                cut_line2->SetLineWidth(2);
                cut_line2->Draw("SAME");
            }
        }

        if(i == 1){
            if(argc > 5 && std::string(argv[5]) == "line_linear"){
                TF1* cut_line2 = new TF1("cut_line2", "0.04*x", 0, 400);
                cut_line2->SetLineWidth(2);
                cut_line2->Draw("SAME");
            }
            if(argc > 5 && std::string(argv[5]) == "line_combined"){
                TLine* cut_line1 = new TLine(20, 0, 20, 15);
                cut_line1->SetLineColor(kRed);
                cut_line1->SetLineWidth(2);
                cut_line1->Draw("SAME");
                TF1* cut_line2 = new TF1("cut_line2", "0.04*x+0.6", 25, 80);
                cut_line2->SetLineColor(kRed);
                cut_line2->SetLineWidth(2);
                cut_line2->Draw("SAME");
            }
        }
        
        latex->DrawLatex(0.50, 0.30, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.50, 0.27, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.50, 0.24, Form("Threshold %d ADC", target_threshold));
        latex->DrawLatex(0.50, 0.21, Form("Entries: %.0f", histograms2d[i]->GetEntries()));
    }

    canvas2d->Update();
    std::string filename2d = "feature_summary_2d_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_threshold_" + std::to_string(target_threshold) + ".png";
    canvas2d->SaveAs((save_dir / filename2d).string().c_str());

    // etc per peak
    auto* canvas_per_peak = PlotSupporter::MakeCanvas2D("canvas_per_peak", 4, 1);
    std::array<TH1*, 4> histograms_per_peak = {hist7, hist8, hist9, hist10};
    for(size_t i = 0; i < histograms_per_peak.size(); ++i){
        canvas_per_peak->cd(i + 1);
        PlotSupporter::SetHistStyle1D(histograms_per_peak[i]);
        if(i == 0){
            histograms_per_peak[i]->GetXaxis()->SetRangeUser(0, 7);
            histograms_per_peak[i]->GetYaxis()->SetRangeUser(0, 2000);
        }
        if(i == 1){
            histograms_per_peak[i]->GetXaxis()->SetRangeUser(0, 0.2);
            histograms_per_peak[i]->GetYaxis()->SetRangeUser(0, 2000);
        }
        if(i == 2){
            histograms_per_peak[i]->GetXaxis()->SetRangeUser(-200, 200);
            histograms_per_peak[i]->GetYaxis()->SetRangeUser(0, 2000);
        }
        if(i == 3){
            histograms_per_peak[i]->GetXaxis()->SetRangeUser(-10, 10);
            histograms_per_peak[i]->GetYaxis()->SetRangeUser(0, 2000);
        }
        histograms_per_peak[i]->Draw("HIST");
        latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));
    }

    canvas_per_peak->Update();
    std::string filename_per_peak = "feature_summary_per_peak_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_threshold_" + std::to_string(target_threshold) + ".png";
    canvas_per_peak->SaveAs((save_dir / filename_per_peak).string().c_str());

    auto* canvas_slope_only = PlotSupporter::MakeCanvas1D("canvas_slope_only", 1, 1);
    PlotSupporter::SetHistStyle1D(hist11);
    hist11->GetXaxis()->SetRangeUser(0, 80);
    hist11->GetYaxis()->SetRangeUser(0, 2000);
    hist11->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
    latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));
    canvas_slope_only->Update();
    std::string filename_slope_only = "feature_summary_slope_only_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_threshold_" + std::to_string(target_threshold) + ".png";
    canvas_slope_only->SaveAs((save_dir / filename_slope_only).string().c_str());

    // Slope and TotVsSlope
    auto* canvas_slope = PlotSupporter::MakeCanvas2D("canvas_slope", 3, 2);
    std::array<TH2D*, 5> histograms_slope = {hist12, hist13, hist14, hist15, hist16};
    for(size_t i = 0; i < histograms_slope.size(); ++i){
        canvas_slope->cd(i + 1);
        PlotSupporter::SetHistStyle2D(histograms_slope[i]);
        if(i == 0){
            histograms_slope[i]->GetXaxis()->SetRangeUser(0, 15);
            histograms_slope[i]->GetYaxis()->SetRangeUser(0, 80);
        }
        if(i == 1){
            histograms_slope[i]->GetXaxis()->SetRangeUser(0, 400);
            histograms_slope[i]->GetYaxis()->SetRangeUser(0, 80);
        }
        if(i == 2){
            histograms_slope[i]->GetXaxis()->SetRangeUser(0, 1000);
            histograms_slope[i]->GetYaxis()->SetRangeUser(0, 80);
        }
        histograms_slope[i]->Draw("COLZ");
        latex->DrawLatex(0.65, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.65, 0.80, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.65, 0.75, Form("Threshold %d ADC", target_threshold));
        latex->DrawLatex(0.65, 0.70, Form("Entries: %.0f / 200000", histograms_slope[i]->GetEntries()));

        if(i == 0){
            TLine* cut_line3 = new TLine(0, 14, 100, 14);
            cut_line3->SetLineColor(kRed);
            cut_line3->SetLineWidth(2);
            cut_line3->Draw("SAME");

            TLine* cut_line4 = new TLine(3.3, 0, 3.3, 100);
            cut_line4->SetLineColor(kRed);
            cut_line4->SetLineWidth(2);
            cut_line4->Draw("SAME");
        }
    }

    canvas_slope->Update();
    std::string filename_slope = "feature_summary_slope_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_threshold_" + std::to_string(target_threshold) + ".png";
    canvas_slope->SaveAs((save_dir / filename_slope).string().c_str());

    return 0;
}