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

    std::array<std::array<TH1D*, 16>, 2> hist_array;
    for(int lgad=0; lgad<2; lgad++){
        for(int ch=0; ch<16; ch++){
            std::string hist_name = "hist_lgad" + std::to_string(lgad) + "_ch" + std::to_string(ch);
            std::string hist_title = Form(";ch %d ADC[ADC counts];Entries", ch);
            hist_array[lgad][ch] = new TH1D(hist_name.c_str(), hist_title.c_str(), 100, 0, 500);
        }
    }

    for(Long64_t entry=0; entry<hd.nentries_; entry++){
        hd.tree_->GetEntry(entry);

        int board = hd.eh_.board;
        int ch = hd.eh_.ch;

        Long64_t current_evt = hd.eh_.evt;

        if(through_event_set.find(current_evt) == through_event_set.end()){
            continue;
        }

        if(board == 0) continue;

        hist_array[Digi2Lgad(ch)][Digi2PixelCh(ch)]->Fill(-hd.eh_.peak_adc);
    }

    PlotSupporter ps(td);
    auto* canvas_front = PlotSupporter::MakeCanvas1D("front_canvas", 4, 4);
    auto* canvas_back = PlotSupporter::MakeCanvas1D("back_canvas", 4, 4);
    for(int ch=0; ch<16; ch++){
        canvas_front->cd(PixelCh2Pad(0, ch));
        hist_array[0][ch]->Draw("HIST");
        canvas_back->cd(PixelCh2Pad(1, ch));
        hist_array[1][ch]->Draw("HIST");
    }

    std::filesystem::path save_dir = ps.MakeSaveDir();
    canvas_front->SaveAs((save_dir / "front_ch_adc_plots.png").string().c_str());
    canvas_back->SaveAs((save_dir / "back_ch_adc_plots.png").string().c_str());
}