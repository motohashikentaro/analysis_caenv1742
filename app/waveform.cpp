#include "./../include/rootfile_analyzer.h"

#include <array>
#include <iostream>

#include <TCanvas.h>
#include <TLatex.h>
#include <TGraph.h>
#include <TStyle.h>

int main(int argc, char* argv[]){
    if(argc != 6){
        std::cerr << "Usage: " << argv[0] << " <input_root_file> <board> <channel> <range_start> <range_end>" << std::endl;
        return 1;
    }
    
    RootfileReader rfr(argv[1]);
    RootfileAnalyzer rfa(rfr.GetRootData());

    int board = std::stoi(argv[2]);
    int ch = std::stoi(argv[3]);

    int range_start = std::stoi(argv[4]);
    int range_end = std::stoi(argv[5]);

    std::array<TGraph*, 1000> graphs;
    graphs = rfa.Waveform(board, ch, range_start, range_end);

    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 600);

    gStyle->SetOptStat(0);

    for(int i=0; i<1000; i++){
        if(graphs[i] == nullptr){
            std::cout << "nullptr at event " << i << std::endl;
            continue;
        }
        if(i==0){
            graphs[i]->Draw();
        }else{
            graphs[i]->Draw("same");
        }
    }

    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);

    latex->DrawLatex(0.70, 0.85, Form("Run %s", (rfr.GetRootData()).run_number_.c_str()));
    latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));

    canvas->SaveAs(("./../result/" + (rfr.GetRootData()).filename_ + "_waveform.png").c_str());

    return 0;
}