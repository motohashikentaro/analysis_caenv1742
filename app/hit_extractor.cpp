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

    FeatureReader fr(argv[1]);
    HitExtractor he(fr.GetFeatureData());

    SelectionConditions::SelectionCondition tracker_condition{"Standard", SelectionConditions::StandardTrackerCondition};
    SelectionConditions::SelectionCondition dut_condition{"Standard", SelectionConditions::StandardDutCondition};
    he.HitExtraction(tracker_condition, dut_condition);

    return 0;
}