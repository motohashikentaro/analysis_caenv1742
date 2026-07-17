#include "./../include/rootfile_analyzer.h"
#include "./../include/channel_map.h"
#include "./../include/hit_selection.h"

#include <iostream>
#include <filesystem>

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>

int main(int argc, char* argv[]){

    RootfileAnalyzer rfa(argv[1]);
    HitSelection hs(rfa.GetRootData());
    hs.HitExtraction(); 
    
    return 0;
}