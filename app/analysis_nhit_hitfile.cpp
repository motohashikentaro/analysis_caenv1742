#include "./../include/hit_reader.h"
#include "./../include/plot_supporter.h"

#include <iostream>
#include <set>
#include <string>
#include <filesystem>

#include <TH1D.h>
#include <TCanvas.h>
#include <TFile.h>

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <hit_root_file>" << std::endl;
        return 1;
    }

    try{
        HitReader hr(argv[1]);
        const HitData& hd = hr.GetHitData();

        PlotSupporter ps(hd);

        TH1D* hist_front = new TH1D(
            "n_hit_ch_front",
            ";n_{hit ch} front;Entries",
            17, -0.5, 16.5
        );

        TH1D* hist_back = new TH1D(
            "n_hit_ch_back",
            ";n_{hit ch} back;Entries",
            17, -0.5, 16.5
        );

        // DUT hit channels
        std::set<int> front_channels;
        std::set<int> back_channels;

        // Strip 4 layers
        bool has_strip_front_x = false;
        bool has_strip_front_y = false;
        bool has_strip_back_x = false;
        bool has_strip_back_y = false;

        Long64_t current_evt = -1;

        auto process_event = [&](){

            const bool is_6through =
                has_strip_front_x &&
                has_strip_front_y &&
                has_strip_back_x &&
                has_strip_back_y &&
                !front_channels.empty() &&
                !back_channels.empty();

            // 6層貫通イベントのみFill
            if(!is_6through) return;

            hist_front->Fill(
                static_cast<int>(front_channels.size())
            );

            hist_back->Fill(
                static_cast<int>(back_channels.size())
            );
        };

        auto clear_event = [&](){

            front_channels.clear();
            back_channels.clear();

            has_strip_front_x = false;
            has_strip_front_y = false;
            has_strip_back_x = false;
            has_strip_back_y = false;
        };


        // =========================
        // Event loop
        // =========================

        for(Long64_t i = 0; i < hd.nentries_; ++i){

            hd.tree_->GetEntry(i);

            const auto& eh = hd.eh_;

            if(current_evt == -1){
                current_evt = eh.evt;
            }

            // event changed
            if(eh.evt != current_evt){

                process_event();
                clear_event();

                current_evt = eh.evt;
            }


            // -------------------------
            // Strip tracker: board 0
            // -------------------------
            if(eh.board == 0){

                if(0 <= eh.ch && eh.ch < 8){
                    has_strip_front_x = true;
                }
                else if(8 <= eh.ch && eh.ch < 16){
                    has_strip_front_y = true;
                }
                else if(16 <= eh.ch && eh.ch < 24){
                    has_strip_back_x = true;
                }
                else if(24 <= eh.ch && eh.ch < 32){
                    has_strip_back_y = true;
                }
            }


            // -------------------------
            // DUT: board 1
            // -------------------------
            else if(eh.board == 1){

                // Front DUT
                if(0 <= eh.ch && eh.ch < 16){
                    front_channels.insert(eh.ch);
                }

                // Back DUT
                else if(16 <= eh.ch && eh.ch < 32){
                    back_channels.insert(eh.ch);
                }
            }
        }


        // 最後のevent
        if(current_evt != -1){
            process_event();
        }


        // =========================
        // Draw
        // =========================

        PlotSupporter::SetHistStyle1D(hist_front);
        PlotSupporter::SetHistStyle1D(hist_back);

        auto* canvas =
            PlotSupporter::MakeCanvas1D(
                "dut_nhit_summary",
                2,
                1
            );

        canvas->cd(1);
        hist_front->Draw("HIST");

        canvas->cd(2);
        hist_back->Draw("HIST");


        // =========================
        // Save
        // =========================

        std::filesystem::path save_dir = ps.MakeSaveDir();

        std::filesystem::path png_path =
            save_dir / "dut_nhit_summary.png";

        std::filesystem::path pdf_path =
            save_dir / "dut_nhit_summary.pdf";

        std::filesystem::path root_path =
            save_dir / "dut_nhit_summary.root";

        canvas->SaveAs(png_path.string().c_str());
        canvas->SaveAs(pdf_path.string().c_str());

        TFile* fout =
            new TFile(root_path.string().c_str(), "RECREATE");

        hist_front->Write();
        hist_back->Write();
        canvas->Write();

        fout->Close();
        delete fout;

        std::cout << "Saved:" << std::endl;
        std::cout << "  " << png_path << std::endl;
        std::cout << "  " << pdf_path << std::endl;
        std::cout << "  " << root_path << std::endl;

    }catch(const std::exception& e){

        std::cerr
            << "Error: "
            << e.what()
            << std::endl;

        return 1;
    }

    return 0;
}