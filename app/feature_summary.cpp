#include "./../include/feature_analyzer.h"
#include "./../include/feature_reader.h"

#include <iostream>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>

int main(int argc, char* argv[]){
    if(argc != 5){
        std::cerr << "Usage: " << argv[0] << " <input_root_file> <board> <ch> <target_threshold>" << std::endl;
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

    // 1D histogram
    TCanvas* canvas = new TCanvas("canvas", "Feature Summary", 2400, 600);
    canvas->Divide(4, 1);

    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);

    canvas->cd(1);
    hist1->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
    latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

    canvas->cd(2);
    hist2->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
    latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

    canvas->cd(3);
    hist3->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
    latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

    canvas->cd(4);
    hist5->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
    latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

    canvas->Update();
    canvas->SaveAs(("./../result/run_" + fr.GetFeatureData().run_number_ + "_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_threshold_" + std::to_string(target_threshold) + "_feature_summary.png").c_str());

    // 2D histogram
    TCanvas* canvas2d = new TCanvas("canvas2d", "Peak vs etc", 1200, 600);
    canvas2d->Divide(2, 1);

    canvas2d->cd(1);
    hist4->GetYaxis()->SetRangeUser(0, 1000);
    hist4->Draw("COLZ");

    canvas2d->cd(2);
    hist6->GetYaxis()->SetRangeUser(0, 14);
    hist6->Draw("COLZ");

    canvas2d->Update();
    canvas2d->SaveAs(("./../result/run_" + fr.GetFeatureData().run_number_ + "_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_threshold_" + std::to_string(target_threshold) + "_feature_summary_2d.png").c_str());

    return 0;
}