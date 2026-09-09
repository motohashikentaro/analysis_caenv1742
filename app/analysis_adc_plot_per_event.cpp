#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/channel_map.h"
#include "./../include/through_event_reader.h"

#include <iostream>
#include <filesystem>

#include <TH1D.h>

int main(int argc, char* argv[]){
    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <hit_root_file> <through_event_root_file>" << std::endl;
        return 1;
    }

    HitReader hr(argv[1]);
    HitData& hd = hr.GetHitData();

    ThroughEventReader tr(argv[2]);
    ThroughEventData& td = tr.GetThroughEventData();

    std::unordered_set<Long64_t> through_event_set;
    for(Long64_t entry=0; entry<td.nentries_; entry++){
        td.tree_->GetEntry(entry);
        through_event_set.insert(td.te_.evt);
    }

    TH1D* front_hist = new TH1D("front_hist", ";front DUT sum ADC[ADC counts]/event;Entries", 100, 0, 5000);
    TH1D* back_hist = new TH1D("back_hist", ";back DUT sum ADC[ADC counts]/event;Entries", 100, 0, 5000);
    TH1D* front_hist_charge = new TH1D("front_hist_charge", ";front DUT sum charge/event [a.u.];Entries", 100, 0, 5000);
    TH1D* back_hist_charge = new TH1D("back_hist_charge", ";back DUT sum charge/event [a.u.];Entries", 100, 0, 5000);

    Long64_t previous_evt = -1;
    double front_sum_adc = 0;
    double back_sum_adc = 0;
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
            front_hist->Fill(front_sum_adc);
            back_hist->Fill(back_sum_adc);
            front_hist_charge->Fill(front_sum_charge);
            back_hist_charge->Fill(back_sum_charge);
            front_sum_adc = 0;
            back_sum_adc = 0;
            front_sum_charge = 0;
            back_sum_charge = 0;
            previous_evt = current_evt;
        }

        if(lgad == 0){
            front_sum_adc += -hd.eh_.peak_adc;
            front_sum_charge += hd.eh_.charge;
        }
        if(lgad == 1){
            back_sum_adc += -hd.eh_.peak_adc;
            back_sum_charge += hd.eh_.charge;
        }
    }

    if(previous_evt != -1){
        front_hist->Fill(front_sum_adc);
        back_hist->Fill(back_sum_adc);
        front_hist_charge->Fill(front_sum_charge);
        back_hist_charge->Fill(back_sum_charge);
    }

    PlotSupporter ps(td);
    std::vector<TH1D*> hists = {front_hist, back_hist, front_hist_charge, back_hist_charge};
    auto* canvas = PlotSupporter::MakeCanvas1D("canvas", 2, 2);
    for(int i=0; i<4; ++i){
        canvas->cd(i+1);
        PlotSupporter::SetHistStyle1D(hists[i]);
        hists[i]->Draw("HIST");
    }
    
    std::filesystem::path save_dir = ps.MakeSaveDir();
    canvas->SaveAs((save_dir / "sum_adc_per_event.png").string().c_str());

    return 0;
}