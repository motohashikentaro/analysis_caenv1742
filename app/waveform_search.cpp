#include "./../include/rootfile_analyzer.h"
#include "./../include/rootfile_reader.h"
#include "./../include/feature_analyzer.h"
#include "./../include/feature_reader.h"

#include <array>
#include <iostream>

#include <TCanvas.h>
#include <TLatex.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TAxis.h>

int main(int argc, char* argv[]){
    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <raw_root_file> <feature_file>" << std::endl;
        return 1;
    }
    
    RootfileReader rfr(argv[1]);
    RootfileAnalyzer rfa(rfr.GetRootData());

    FeatureReader fr(argv[2]);
    FeatureAnalyzer fa(fr.GetFeatureData());

    constexpr int board = 1;
    constexpr int ch = 21;
    constexpr size_t max_events = 20;
    constexpr int range_start = 110;
    constexpr int range_end = 220;
    
    std::vector<Long64_t> target_events = fa.SelectEvents(board, ch, max_events, [](const EvtFeature& ef){
        constexpr size_t threshold_idx = 3;

        const double peak_adc = -ef.peak_adc;
        const int peak_time = ef.peak_time;
        const double tot = ef.tots[threshold_idx];
        const double charge = ef.charges[threshold_idx];

        if(peak_adc < 60 || peak_adc > 70) return false;
        if(tot < 3 || tot > 5) return false;
        // if(peak_adc < 60 || peak_adc > 75) return false;
        // if(charge < 40 || charge > 100) return false;
        // if(peak_adc < 60 || peak_adc > 80) return false;
        // if(tot < 2.5 || tot > 4) return false;

        return true;
    });

    std::vector<TGraph*> graphs = rfa.Waveform(board, ch, target_events, range_start, range_end);

    TCanvas* canvas = new TCanvas("canvas", "Waveforms", 1000, 700);

    for(size_t i = 0; i < graphs.size(); i++){
        if(i == 0){
            graphs[i]->GetYaxis()->SetRangeUser(-100, 50);
            graphs[i]->Draw("AL");
        }
        else{
            graphs[i]->GetYaxis()->SetRangeUser(-100, 50);
            graphs[i]->Draw("L SAME");
        }
    }

    canvas->SaveAs("./../result/waveforms.png");
    
    return 0;
}