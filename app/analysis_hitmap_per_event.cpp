#include "./../include/hit_reader.h"
#include "./../include/through_event_reader.h"
#include "./../include/channel_map.h"
#include "./../include/plot_supporter.h"

#include <unordered_set>
#include <iostream>

#include <TFile.h>
#include <TH2D.h>

int main(int argc, char* argv[]){

    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " <hit_root_file> <through_event_root_file>" << std::endl;
        return 1;
    }

    constexpr int max_evt = 100;

    HitReader hr(argv[1]);
    ThroughEventReader tr(argv[2]);

    HitData& hd = hr.GetHitData();
    ThroughEventData& td = tr.GetThroughEventData();

    PlotSupporter ps(td);
    TFile* file = TFile::Open((ps.MakeSaveDir() / "hitmap_per_event.root").c_str(), "RECREATE");

    std::unordered_set<Long64_t> through_event_set;
    Long64_t td_evt;
    td.tree_->SetBranchAddress("evt", &td_evt);
    for(Long64_t i = 0; i < max_evt; ++i){
        td.tree_->GetEntry(i);
        through_event_set.insert(td_evt);
    }

    Long64_t hd_evt;
    int hd_board;
    int hd_ch;
    double hd_charge;

    hd.tree_->SetBranchAddress("evt", &hd_evt);
    hd.tree_->SetBranchAddress("board", &hd_board);
    hd.tree_->SetBranchAddress("ch", &hd_ch);
    hd.tree_->SetBranchAddress("charge", &hd_charge);

    Long64_t previous_evt = -1;
    TH2D* front_hitmap = nullptr;
    TH2D* back_hitmap = nullptr;

    for(Long64_t i=0; i<hd.nentries_; ++i){
        hd.tree_->GetEntry(i);

        if(through_event_set.find(hd_evt) == through_event_set.end()) continue;

        if(previous_evt == -1){
            front_hitmap = new TH2D(Form("front_evt_%lld", hd_evt), Form("Front Hitmap for Event %lld", hd_evt), 4, 0, 4, 4, 0, 4);
            back_hitmap = new TH2D(Form("back_evt_%lld", hd_evt), Form("Back Hitmap for Event %lld", hd_evt), 4, 0, 4, 4, 0, 4);

            previous_evt = hd_evt;
        }

        if(previous_evt != hd_evt){
            file->cd();
            front_hitmap->Write();
            back_hitmap->Write();

            delete front_hitmap;
            delete back_hitmap;

            front_hitmap = nullptr;
            back_hitmap = nullptr;

            previous_evt = hd_evt;

            front_hitmap = new TH2D(Form("front_evt_%lld", hd_evt), Form("Front Hitmap for Event %lld", hd_evt), 4, 0, 4, 4, 0, 4);
            back_hitmap = new TH2D(Form("back_evt_%lld", hd_evt), Form("Back Hitmap for Event %lld", hd_evt), 4, 0, 4, 4, 0, 4);
        }

        if(hd_board != 1) continue;

        const PixelPosition pos = Digi2PixelPosition(hd_ch);
        const int lgad = Digi2Lgad(hd_ch);

        if(pos.x == -1 || pos.y == -1) continue;

        if(lgad == 0){
            front_hitmap->Fill(pos.x, pos.y, hd_charge);
        }else if(lgad == 1){
            back_hitmap->Fill(pos.x, pos.y, hd_charge);
        }
    }

    if(front_hitmap){
        file->cd();
        front_hitmap->Write();
        back_hitmap->Write();

        delete front_hitmap;
        delete back_hitmap;
    }

    file->Close();
    delete file;

    return 0;
}
