#include "./../include/hit_extractor.h"
#include "./../include/feature_reader.h"
#include "./../include/selection_conditions.h"

#include <filesystem>
#include <stdexcept>

#include <TFile.h>
#include <TTree.h>
#include <TNamed.h>

void HitExtractor::HitExtraction(const SelectionConditions::SelectionCondition& tracker_condition, const SelectionConditions::SelectionCondition& dut_condition){
    EvtHit eh;

    // output path
    const std::string hit_condition = std::string(tracker_condition.name) + "-" + std::string(dut_condition.name);
    const std::filesystem::path output_dir = std::filesystem::path("./../data/hit") / fd_.feature_condition_ / hit_condition;
    std::filesystem::create_directories(output_dir);
    const std::filesystem::path output_path = output_dir / ("hit_" + fd_.run_number_ + ".root");

    // create root file
    TFile* fout = new TFile(output_path.string().c_str(), "RECREATE");

    // metadata
    TNamed run_number("run_number", fd_.run_number_.c_str());
    TNamed feature_condition_metadata("feature_condition", fd_.feature_condition_.c_str());
    TNamed tracker_condition_metadata("tracker_condition", tracker_condition.name);
    TNamed dut_condition_metadata("dut_condition", dut_condition.name);

    // create tree
    TTree* tree = new TTree("tree", "Hit events");
    tree->Branch("evt", &eh.evt);
    tree->Branch("board", &eh.board);
    tree->Branch("ch", &eh.ch);
    tree->Branch("pedestal", &eh.pedestal);
    tree->Branch("peak_adc", &eh.peak_adc);
    tree->Branch("peak_time", &eh.peak_time);
    tree->Branch("raise_time", &eh.raise_time);
    tree->Branch("fall_time", &eh.fall_time);
    tree->Branch("charge", &eh.charge);
    tree->Branch("tot", &eh.tot);
    tree->Branch("raise_slope", &eh.raise_slope);

    // read root file
    for(Long64_t entry=0; entry<fd_.nentries_; entry++){
        fd_.tree_->GetEntry(entry);

        // tracker hit selection
        if(fd_.ef_.board == 0){
            auto threshold_idx = tracker_condition.condition(fd_.ef_);
            if(!threshold_idx) continue;

            eh.evt = fd_.ef_.evt;
            eh.board = fd_.ef_.board;
            eh.ch = fd_.ef_.ch;
            eh.pedestal = fd_.ef_.pedestal;
            eh.peak_adc = fd_.ef_.peak_adc;
            eh.peak_time = fd_.ef_.peak_time;
            eh.raise_time = fd_.ef_.raise_times[*threshold_idx];
            eh.fall_time = fd_.ef_.fall_times[*threshold_idx];
            eh.charge = fd_.ef_.charges[*threshold_idx];
            eh.tot = fd_.ef_.tots[*threshold_idx];
            eh.raise_slope = fd_.ef_.raise_slopes[*threshold_idx];
            tree->Fill();
        }

        // dut hit selection
        if(fd_.ef_.board == 1){
            auto threshold_idx = dut_condition.condition(fd_.ef_);
            if(!threshold_idx) continue;

            eh.evt = fd_.ef_.evt;
            eh.board = fd_.ef_.board;
            eh.ch = fd_.ef_.ch;
            eh.pedestal = fd_.ef_.pedestal;
            eh.peak_adc = fd_.ef_.peak_adc;
            eh.peak_time = fd_.ef_.peak_time;
            eh.raise_time = fd_.ef_.raise_times[*threshold_idx];
            eh.fall_time = fd_.ef_.fall_times[*threshold_idx];
            eh.charge = fd_.ef_.charges[*threshold_idx];
            eh.tot = fd_.ef_.tots[*threshold_idx];
            eh.raise_slope = fd_.ef_.raise_slopes[*threshold_idx];
            tree->Fill();
        }
    }

    fout->cd();
    tree->Write();
    run_number.Write();
    feature_condition_metadata.Write();
    tracker_condition_metadata.Write();
    dut_condition_metadata.Write();
    fout->Close();

    delete fout;
}