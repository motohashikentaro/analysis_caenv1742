#include "./../include/hit_analyzer.h"
#include "./../include/hit_reader.h"

#include <iostream>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>

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

    // 1D histogram
    TCanvas* canvas = new TCanvas("canvas", "Hit Summary", 2400, 600);
    canvas->Divide(4, 1);

    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);

    canvas->cd(1);
    hist1->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (hr.GetHitData()).run_number_condition_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
    
    canvas->cd(2);
    hist2->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (hr.GetHitData()).run_number_condition_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));

    canvas->cd(3);
    hist3->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (hr.GetHitData()).run_number_condition_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));

    canvas->cd(4);
    hist5->Draw("HIST");
    latex->DrawLatex(0.70, 0.85, Form("Run %s", (hr.GetHitData()).run_number_condition_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));

    canvas->Update();
    canvas->SaveAs(("./../result/run_" + (hr.GetHitData()).run_number_condition_ + "_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_hit_summary.png").c_str());

    // 2D histogram
    TCanvas* canvas2d = new TCanvas("canvas2d", "Hit Summary 2D", 1200, 600);
    canvas2d->Divide(2, 1);

    canvas2d->cd(1);
    hist4->GetYaxis()->SetRangeUser(0, 1000);
    hist4->Draw("COLZ");
    
    canvas2d->cd(2);
    hist6->GetYaxis()->SetRangeUser(0, 14);
    hist6->Draw("COLZ");

    canvas2d->Update();
    canvas2d->SaveAs(("./../result/run_" + (hr.GetHitData()).run_number_condition_ + "_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_hit_summary_2d.png").c_str());

    return 0;
}