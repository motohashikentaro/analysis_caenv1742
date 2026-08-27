#include "./../include/hit_extractor.h"
#include "./../include/feature_reader.h"
#include "./../include/selection_conditions.h"

#include <TFile.h>
#include <TTree.h>

void HitExtractor::HitExtraction(const SelectionConditions::SelectionCondition& tracker_condition, const SelectionConditions::SelectionCondition& dut_condition){
    EvtHit eh;

    // create root file
    TFile* fout = new TFile(("./../data/hit/hit_" + fd_.run_number_ + "-" + tracker_condition.name + "-" + dut_condition.name + ".root").c_str(), "RECREATE");
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
    fout->Close();

    delete fout;
}