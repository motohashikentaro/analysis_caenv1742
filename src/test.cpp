#include <iostream>
#include <algorithm>

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>

#include "./../include/rootfile_analyzer.h"
#include "./../include/analysis_process.h"

int main(int argc, char* argv[]){

    RootfileAnalyzer rfa(argv[1]);
    for(Long64_t evt=0; evt<rfa.GetRootData().nentries_; evt++){
        rfa.GetRootData().tree_->GetEntry(evt);


        for(int sample=0; sample<rfa.GetRootData().nsample; sample++){
            std::cout << rfa.GetRootData().ev_.amp[1][24][sample] << " ";
        }
        std::cout << std::endl;

        break;  // Just read the first event for testing
    }
    return 0;
}