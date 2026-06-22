#include <iostream>
#include <filesystem>

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>

#include "./../include/rootfile_analyzer.h"
#include "./../include/analysis_process.h"
#include "./../include/channel_map.h"
#include "./../include/save_objects.h"

int main(int argc, char* argv[]){

    RootfileAnalyzer rfa(argv[1]);
    AnalysisProcess ap(rfa.GetRootData());
    ap.SeparateWaveform(1, 24);
    
    return 0;
}