#include "./../include/hit_analyzer.h"
#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"

#include <iostream>
#include <array>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TColor.h>

int main(int argc, char* argv[]){
    if(argc != 4){
        std::cerr << "Usage: " << argv[0] << " <input_root_file> <board> <ch>" << std::endl;
        return 1;
    }
    HitReader hr(argv[1]);
    HitAnalyzer ha(hr.GetHitData());

    int board = std::stoi(argv[2]);
    int ch = std::stoi(argv[3]);

    TH1D* hist1 = ha.PeakDistro(board, ch);
    TH1D* hist2 = ha.PeaktimeDistro(board, ch);
    TH1D* hist3 = ha.ChargeDistro(board, ch); // Assuming target_threshold is not needed for this example
    TH2D* hist4 = ha.PeakVsCharge(board, ch); // Assuming target_threshold is not needed for this example
    TH1D* hist5 = ha.TotDistro(board, ch); // Assuming target_threshold is not needed for this example
    TH2D* hist6 = ha.PeakVsTot(board, ch); // Assuming target_threshold is not needed for this example

    gStyle->SetOptStat(0);
    auto pramary_color = TColor::GetColor("#008899");  
    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);

    // 1D histogram
    auto* canvas = PlotSupporter::MakeCanvas1D("canvas", 4, 1);
    std::array<TH1*, 4> histograms = {hist1, hist2, hist3, hist5};
    for(size_t i = 0; i < histograms.size(); ++i){
        canvas->cd(i + 1);
        PlotSupporter::SetHistStyle1D(histograms[i]);
        if(i == 2) histograms[i]->GetXaxis()->SetRangeUser(1, 1000);
        if(i == 3) histograms[i]->GetXaxis()->SetRangeUser(1, 15);
        histograms[i]->Draw("HIST");
        latex->DrawLatex(0.50, 0.80, Form("Run %s", (hr.GetHitData()).run_number_condition_.c_str()));
        latex->DrawLatex(0.50, 0.77, Form("Board %d, Ch %d", board, ch));
    }

    canvas->Update();
    canvas->SaveAs(("./../result/run_" + (hr.GetHitData()).run_number_condition_ + "_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_hit_summary.png").c_str());

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
        latex->DrawLatex(0.50, 0.30, Form("Run %s", (hr.GetHitData()).run_number_condition_.c_str()));
        latex->DrawLatex(0.50, 0.27, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.50, 0.24, Form("Entries: %.0f", histograms2d[i]->GetEntries()));
    }

    canvas2d->Update();
    canvas2d->SaveAs(("./../result/run_" + (hr.GetHitData()).run_number_condition_ + "_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_hit_summary_2d.png").c_str());

    return 0;
}