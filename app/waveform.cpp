#include "./../include/rootfile_analyzer.h"
#include "./../include/rootfile_reader.h"
#include "./../include/plot_supporter.h"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TLine.h>
#include <TBox.h>
#include <TLatex.h>

int main(int argc, char* argv[]){
    if(argc != 4){
        std::cerr << "Usage: " << argv[0]
                  << " <raw_root_file> <board> <ch>" << std::endl;
        return 1;
    }

    TLatex* latex = new TLatex();
    latex->SetNDC();

    RootfileReader rfr(argv[1]);
    RootfileAnalyzer rfa(rfr.GetRootData());

    const int board = std::stoi(argv[2]);
    const int ch = std::stoi(argv[3]);

    constexpr Long64_t max_events = 1000;
    constexpr int range_start = 0;
    constexpr int range_end = 1024;

    // 最初の1000イベント
    std::vector<Long64_t> target_events;
    target_events.reserve(max_events);

    for(Long64_t evt = 0; evt < max_events; ++evt){
        target_events.push_back(evt);
    }

    // 波形取得
    std::vector<TGraph*> graphs =
        rfa.Waveform(board, ch, target_events, range_start, range_end);

    TCanvas* canvas =
        new TCanvas("canvas", "Waveforms", 1000, 700);

    // 1000イベントを重ね描き
    for(size_t i = 0; i < graphs.size(); ++i){
        graphs[i]->GetYaxis()->SetRangeUser(-200, 50);

        if(i == 0){
            graphs[i]->Draw("AL");
        }else{
            graphs[i]->Draw("L SAME");
        }
    }

    TBox* ped_box = new TBox(10, -200, 50, 50);
    ped_box->SetFillColorAlpha(kBlue, 0.2);
    ped_box->SetLineColor(kBlue);
    ped_box->Draw("SAME");

    TBox* signal_box = new TBox(110, -200, 220, 50);
    signal_box->SetFillColorAlpha(kRed, 0.2);
    signal_box->SetLineColor(kRed);
    signal_box->Draw("SAME");

    latex->DrawLatex(0.15, 0.85, "Run: 5088");
    latex->DrawLatex(0.15, 0.80, Form("Board: %d, Ch: %d", board, ch));
    latex->DrawLatex(0.15, 0.75, Form("Events: %lld", max_events));
    latex->DrawLatex(0.15, 0.70, Form("2 GeV, 2 W plates"));

    PlotSupporter ps(rfr.GetRootData());
    std::filesystem::path save_dir = ps.MakeSaveDir();

    canvas->SaveAs(
        (save_dir / "waveforms.png").string().c_str()
    );

    return 0;
}
