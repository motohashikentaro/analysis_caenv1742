#include "./../include/hit_extractor.h"
#include "./../include/feature_reader.h"
#include "./../include/selection_conditions.h"

#include <iostream>
#include <filesystem>

#include <TFile.h>
#include <TTree.h>
#include <TGraph.h>
#include <TCanvas.h>

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <feature_root_file>" << std::endl;
        return 1;
    }

    FeatureReader fr(argv[1]);
    HitExtractor he(fr.GetFeatureData());

    const auto& tracker_condition = SelectionConditions::CorrectThresTracker;
    const auto& dut_condition = SelectionConditions::ForLast8chDut;

    he.HitExtraction(tracker_condition, dut_condition);

    return 0;
}