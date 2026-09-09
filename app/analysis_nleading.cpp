#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/channel_map.h"
#include "./../include/through_event_reader.h"

#include <iostream>
#include <filesystem>

#include <TH1D.h>

int main(int argc, char* argv[]){
    if(argc != 4){
        std::cerr << "Usage: " << argv[0] << " <hit_root_file> <through_event_root_file> <lead_threshold>" << std::endl;
        return 1;
    }


    HitReader hr(argv[1]);
    HitData& hd = hr.GetHitData();

    ThroughEventReader tr(argv[2]);
    ThroughEventData& td = tr.GetThroughEventData();

    int lead_threshold = std::stoi(argv[3]);

    std::unordered_set<Long64_t> through_event_set;
    for(Long64_t entry=0; entry<td.nentries_; entry++){
        td.tree_->GetEntry(entry);
        through_event_set.insert(td.te_.evt);
    }

    TH1D* front_hist = new TH1D("front_hist", ";n leading ch;Entries", 17, 0, 17);
    TH1D* back_hist = new TH1D("back_hist", ";n leading ch;Entries", 17, 0, 17);

    std::vector<EvtHit> hits;

    Long64_t previous_evt = -1;

    for(Long64_t entry=0; entry<hd.nentries_; entry++){
        hd.tree_->GetEntry(entry);

        Long64_t current_evt = hd.eh_.evt;

        if(through_event_set.find(current_evt) == through_event_set.end()){
            continue;
        }

        int board = hd.eh_.board;
        int ch = hd.eh_.ch;

        if(board == 0) continue;

        if(previous_evt == -1){
            previous_evt = current_evt;
        }

        if(previous_evt != current_evt){
            int front_n_leading = 0;
            int back_n_leading = 0;

            for(const auto& hit: hits){
                if(hit.peak_adc < -lead_threshold){
                    int lgad = Digi2Lgad(hit.ch);
                    if(lgad == 0){
                        front_n_leading++;
                    }else if(lgad == 1){
                        back_n_leading++;
                    }
                }
            }
            front_hist->Fill(front_n_leading);
            back_hist->Fill(back_n_leading);

            previous_evt = current_evt;
            hits.clear();
        }


        hits.push_back(hd.eh_);
    }
    int front_n_leading = 0;
    int back_n_leading = 0;
    for(const auto& hit: hits){
        if(hit.peak_adc < -lead_threshold){
            int lgad = Digi2Lgad(hit.ch);
            if(lgad == 0){
                front_n_leading++;
            }else if(lgad == 1){
                back_n_leading++;
            }
        }
    }
    front_hist->Fill(front_n_leading);
    back_hist->Fill(back_n_leading);

    PlotSupporter ps(td);
    auto* canvas = PlotSupporter::MakeCanvas1D("canvas", 2, 1);
    PlotSupporter::SetHistStyle1D(front_hist);
    PlotSupporter::SetHistStyle1D(back_hist);
    canvas->cd(1);
    front_hist->Draw("HIST");
    canvas->cd(2);
    back_hist->Draw("HIST");

    std::filesystem::path save_dir = ps.MakeSaveDir();

    canvas->SaveAs((save_dir / "n_leading_ch.png").c_str());

    return 0;

}