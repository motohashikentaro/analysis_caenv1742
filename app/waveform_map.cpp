#include "./../include/rootfile_analyzer.h"
#include "./../include/channel_map.h"
#include "./../include/rootfile_reader.h"

#include <iostream>

#include <TCanvas.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TAxis.h>

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <input_root_file>" << std::endl;
        return 1;
    }

    RootfileReader rfr(argv[1]);
    RootfileAnalyzer rfa(rfr.GetRootData());

    gStyle->SetOptStat(0);

    // 4 × 4 × 2 = 32 pads
    TCanvas* canvas1 = new TCanvas("canvas1", "Waveform DUT Front Map", 1600, 1600);
    TCanvas* canvas2 = new TCanvas("canvas2", "Waveform DUT Back Map", 1600, 1600);
    TCanvas* canvas3 = new TCanvas("canvas3", "Waveform Strip Map", 3200, 1600);

    canvas1->Divide(4, 4);
    canvas2->Divide(4, 4);
    canvas3->Divide(8, 4);

    for(int ch=0; ch<32; ch++){

        PixelPosition pos = Digi2PixelPosition(ch);
        int lgad = Digi2Lgad(ch);

        if(lgad == 0) canvas1->cd(pos.x + 1 + pos.y * 4);
        if(lgad == 1) canvas2->cd(pos.x + 1 + pos.y * 4);

        auto graphs = rfa.Waveform(1, ch);

        bool first = true;
        for(auto g : graphs){

            if(g == nullptr) continue;

            if(first){
                g->SetTitle(Form("Ch %d", ch));
                g->GetXaxis()->SetTitle("Sample");
                g->GetYaxis()->SetTitle("ADC");
                g->Draw("AL");
                first = false;
            }
            else{
                g->Draw("L SAME");
            }
        }

        canvas3->cd(ch + 1);

        auto graphs_strip = rfa.Waveform(0, ch);

        first = true;
        for(auto g : graphs_strip){
            if(g == nullptr) continue;

            if(first){
                g->SetTitle(Form("Ch %d", ch));
                g->GetXaxis()->SetTitle("Sample");
                g->GetYaxis()->SetTitle("ADC");
                g->Draw("AL");
                first = false;
            }
            else{
                g->Draw("L SAME");
            }
        }
    }

    canvas1->SaveAs(("./../result/" + rfr.GetRootData().filename_ + "_waveform_front.png").c_str());
    canvas2->SaveAs(("./../result/" + rfr.GetRootData().filename_ + "_waveform_back.png").c_str());
    canvas3->SaveAs(("./../result/" + rfr.GetRootData().filename_ + "_waveform_strip.png").c_str());

    return 0;
}