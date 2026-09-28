#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/channel_map.h"
#include "./../include/through_event_reader.h"

#include <iostream>
#include <filesystem>

#include <TH1D.h>

int main(int argc, char* argv[]){

    HitReader hr(argv[1]);
    HitData& hd = hr.GetHitData();

    HitReader hr_ali("/home/motohashi/work/analysis_caenv1742/data/hit/correct_threshold/CorrectThresTracker-CorrectThresDut/hit_05434-05438.root");
    HitData& hd_ali = hr_ali.GetHitData();

    ThroughEventReader tr(argv[2]);
    ThroughEventData& td = tr.GetThroughEventData();

    ThroughEventReader tr_ali("/home/motohashi/work/analysis_caenv1742/data/through_event/correct_threshold/CorrectThresTracker-CorrectThresDut/NewAlignmentStrip-Shower2HitNewAlignment/through_event_05434-05438.root");
    ThroughEventData& td_ali = tr_ali.GetThroughEventData();

    // alignment run
    std::unordered_set<Long64_t> through_event_set_ali;

    for(Long64_t entry = 0; entry < td_ali.nentries_; ++entry){
        td_ali.tree_->GetEntry(entry);
        through_event_set_ali.insert(td_ali.te_.evt);
    }

    double ali_front_total = 0.0;
    double ali_back_total = 0.0;
    Long64_t ali_n_event = 0;

    Long64_t previous_evt_ali = -1;
    double front_sum_ali = 0.0;
    double back_sum_ali = 0.0;

    for(Long64_t entry = 0; entry < hd_ali.nentries_; ++entry){
        hd_ali.tree_->GetEntry(entry);

        const Long64_t current_evt = hd_ali.eh_.evt;

        if(through_event_set_ali.find(current_evt) == through_event_set_ali.end()){
            continue;
        }

        if(hd_ali.eh_.board == 0) continue;

        if(previous_evt_ali == -1){
            previous_evt_ali = current_evt;
        }

        if(previous_evt_ali != current_evt){

            if(front_sum_ali > 0.0 && back_sum_ali > 0.0){
                ali_front_total += front_sum_ali;
                ali_back_total += back_sum_ali;
                ++ali_n_event;
            }

            front_sum_ali = 0.0;
            back_sum_ali = 0.0;
            previous_evt_ali = current_evt;
        }

        const int lgad = Digi2Lgad(hd_ali.eh_.ch);

        if(lgad == 0){
            front_sum_ali += hd_ali.eh_.charge;
        }else if(lgad == 1){
            back_sum_ali += hd_ali.eh_.charge;
        }
    }

    if(previous_evt_ali != -1 && front_sum_ali > 0.0 && back_sum_ali > 0.0){
        ali_front_total += front_sum_ali;
        ali_back_total += back_sum_ali;
        ++ali_n_event;
    }

    const double ali_front_mean = ali_front_total / ali_n_event;
    const double ali_back_mean  = ali_back_total  / ali_n_event;

    const double normalization_factor =
        ali_front_mean / ali_back_mean;



    // main run
    std::unordered_set<Long64_t> through_event_set;
    for(Long64_t entry=0; entry<td.nentries_; entry++){
        td.tree_->GetEntry(entry);
        through_event_set.insert(td.te_.evt);
    }


    TH1D* back_per_front_hist = new TH1D("back_per_front_hist", ";Normalized Charge Ratio;Entries", 50, 0, 100);

    Long64_t previous_evt = -1;
    double front_sum_charge = 0;
    double back_sum_charge = 0;

    for(Long64_t entry=0; entry<hd.nentries_; entry++){
        hd.tree_->GetEntry(entry);

        Long64_t current_evt = hd.eh_.evt;

        if(through_event_set.find(current_evt) == through_event_set.end()){
            continue;
        }

        int board = hd.eh_.board;
        int ch = hd.eh_.ch;

        if(board == 0) continue;

        int lgad = Digi2Lgad(ch);

        if(previous_evt == -1){
            previous_evt = current_evt;
        }

        if(previous_evt != current_evt){
            if(front_sum_charge > 0.0 && back_sum_charge > 0.0){
                back_per_front_hist->Fill(
                    (back_sum_charge / front_sum_charge) * normalization_factor
                );
            }

            front_sum_charge = 0;
            back_sum_charge = 0;
            previous_evt = current_evt;
        }

        if(lgad == 0){
            front_sum_charge += hd.eh_.charge;
        }
        if(lgad == 1){
            back_sum_charge += hd.eh_.charge;
        }
    }

    if(previous_evt != -1 &&
    front_sum_charge > 0.0 &&
    back_sum_charge > 0.0){

        back_per_front_hist->Fill(
            (back_sum_charge / front_sum_charge) * normalization_factor
        );
    }

    PlotSupporter ps(td);
    std::vector<TH1D*> hists = {back_per_front_hist};
    auto* canvas = PlotSupporter::MakeCanvas1D("canvas", 1, 1);
    for(int i=0; i<1; ++i){
        canvas->cd(i+1);
        PlotSupporter::SetHistStyle1D(hists[i]);
        hists[i]->GetYaxis()->SetRangeUser(0, 200);
        hists[i]->Draw("HIST");
    }
    
    std::filesystem::path save_dir = ps.MakeSaveDir();
    canvas->SaveAs((save_dir / "sum_adc_per_event.png").string().c_str());

}