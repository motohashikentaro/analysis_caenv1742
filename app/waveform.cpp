#include "./../include/analysis_process.h"

#include <array>
#include <iostream>

#include <TCanvas.h>
#include <TGraph.h>

int main(int argc, char* argv[]){
    RootfileAnalyzer rfa(argv[1]);
    AnalysisProcess ap(rfa.GetRootData());

    std::array<TGraph*, 500> graphs;
    graphs = ap.Waveform(1, 24);

    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 600);
    for(int i=0; i<500; i++){
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

    canvas->SaveAs(("./../result/" + (rfa.GetRootData()).filename_ + "_waveform.png").c_str());

    return 0;
}