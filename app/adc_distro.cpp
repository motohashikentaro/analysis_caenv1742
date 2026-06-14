#include <iostream>
#include <filesystem>

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>

#include "./../include/rootfile_analyzer.h"

int main(int argc, char* argv[]){

    RootfileAnalyzer rfa(argv[1]);
    
    std::cout << rfa.GetRootData().nentries_ << std::endl;

    rfa.EventLoop();

    return 0;
}