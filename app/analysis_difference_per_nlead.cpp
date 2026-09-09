#include "./../include/hit_reader.h"
#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/channel_map.h"
#include "./../include/run_map.h"

#include <iostream>
#include <filesystem>
#include <string>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <cmath>

#include <TH1D.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLatex.h>


int main(int argc, char* argv[]){

    if(argc != 4){
        std::cerr
            << "Usage: " << argv[0]
            << " <hit_root_file> <through_event_root_file> <lead_threshold>"
            << std::endl;
        return 1;
    }


    // ============================================================
    // Read files
    // ============================================================

    HitReader hr(argv[1]);
    HitData& hd = hr.GetHitData();

    ThroughEventReader tr(argv[2]);
    ThroughEventData& td = tr.GetThroughEventData();

    const int lead_threshold = std::stoi(argv[3]);


    // ============================================================
    // Make through-event set
    // ============================================================

    std::unordered_set<Long64_t> through_event_set;

    for(Long64_t entry = 0; entry < td.nentries_; ++entry){
        td.tree_->GetEntry(entry);
        through_event_set.insert(td.te_.evt);
    }


    // ============================================================
    // Count leading channels for each event
    //
    // n_leading_map[evt][0] : front
    // n_leading_map[evt][1] : back
    // ============================================================

    std::unordered_map<Long64_t, std::array<int, 2>> n_leading_map;

    for(Long64_t entry = 0; entry < hd.nentries_; ++entry){

        hd.tree_->GetEntry(entry);

        const Long64_t evt = hd.eh_.evt;

        // through event のみを見る
        if(through_event_set.find(evt) == through_event_set.end()){
            continue;
        }

        // DUTのみ
        if(hd.eh_.board != 1){
            continue;
        }

        // leading threshold
        if(hd.eh_.peak_adc >= -lead_threshold){
            continue;
        }

        const int lgad = Digi2Lgad(hd.eh_.ch);

        if(lgad == 0){
            ++n_leading_map[evt][0];
        }else if(lgad == 1){
            ++n_leading_map[evt][1];
        }
    }


    // ============================================================
    // Histograms
    //
    // coordinate index:
    //   0 = front x
    //   1 = back  x
    //   2 = front y
    //   3 = back  y
    //
    // category:
    //   0 = n_leading == 1
    //   1 = n_leading >= 2
    // ============================================================

    std::array<std::string, 2> axes = {"x", "y"};
    std::array<std::string, 2> sides = {"front", "back"};

    constexpr size_t n_category = 2;

    std::array<std::array<TH1D*, n_category>, 4> hist_array{};
    std::array<std::array<TF1*, n_category>, 4> fit_array{};

    const std::array<std::string, n_category> category_name = {
        "nlead1",
        "nlead_ge2"
    };

    const std::array<std::string, n_category> category_title = {
        "n_{leading} = 1",
        "n_{leading} #geq 2"
    };


    size_t index = 0;

    for(const auto& axis : axes){
        for(const auto& side : sides){

            for(size_t category = 0; category < n_category; ++category){

                const std::string hist_name =
                    "hist_" + side + "_" + axis + "_" + category_name[category];

                const std::string hist_title =
                    ";" + axis + "_{reco} - "
                    + axis + "_{extrapolated} [mm];Entries";

                hist_array[index][category] = new TH1D(
                    hist_name.c_str(),
                    hist_title.c_str(),
                    32,
                    -3,
                    3
                );
            }

            ++index;
        }
    }


    // ============================================================
    // Fill difference histograms
    // ============================================================

    for(Long64_t entry = 0; entry < td.nentries_; ++entry){

        td.tree_->GetEntry(entry);

        const Long64_t evt = td.te_.evt;

        auto map_it = n_leading_map.find(evt);

        // mapに無い場合は、leading thresholdを超えたDUT hitが0
        if(map_it == n_leading_map.end()){
            continue;
        }

        const int front_n_leading = map_it->second[0];
        const int back_n_leading  = map_it->second[1];


        // --------------------------------------------------------
        // Front
        // --------------------------------------------------------

        if(front_n_leading == 1){

            hist_array[0][0]->Fill(
                td.te_.dut_position_front_x
                - td.te_.extrapolated_front_x
            );

            hist_array[2][0]->Fill(
                td.te_.dut_position_front_y
                - td.te_.extrapolated_front_y
            );

        }else if(front_n_leading >= 2){

            hist_array[0][1]->Fill(
                td.te_.dut_position_front_x
                - td.te_.extrapolated_front_x
            );

            hist_array[2][1]->Fill(
                td.te_.dut_position_front_y
                - td.te_.extrapolated_front_y
            );
        }


        // --------------------------------------------------------
        // Back
        // --------------------------------------------------------

        if(back_n_leading == 1){

            hist_array[1][0]->Fill(
                td.te_.dut_position_back_x
                - td.te_.extrapolated_back_x
            );

            hist_array[3][0]->Fill(
                td.te_.dut_position_back_y
                - td.te_.extrapolated_back_y
            );

        }else if(back_n_leading >= 2){

            hist_array[1][1]->Fill(
                td.te_.dut_position_back_x
                - td.te_.extrapolated_back_x
            );

            hist_array[3][1]->Fill(
                td.te_.dut_position_back_y
                - td.te_.extrapolated_back_y
            );
        }
    }


    // ============================================================
    // Gaussian fit
    // ============================================================

    for(size_t i = 0; i < 4; ++i){

        for(size_t category = 0; category < n_category; ++category){

            TH1D* hist = hist_array[i][category];

            if(hist->GetEntries() == 0){
                continue;
            }

            const int max_bin = hist->GetMaximumBin();
            const double peak_position = hist->GetBinCenter(max_bin);

            const double fit_min = peak_position - 1.5;
            const double fit_max = peak_position + 1.5;

            const std::string fit_name =
                "fit_" + std::to_string(i)
                + "_" + category_name[category];

            fit_array[i][category] = new TF1(
                fit_name.c_str(),
                "gaus",
                fit_min,
                fit_max
            );

            fit_array[i][category]->SetParameters(
                hist->GetMaximum(),
                peak_position,
                0.5
            );

            hist->Fit(fit_array[i][category], "RQ");
        }
    }


    // ============================================================
    // Draw
    // ============================================================

    gStyle->SetOptStat(0);

    PlotSupporter ps(td);

    const std::filesystem::path save_dir = ps.MakeSaveDir();


    // ============================================================
    // n_leading == 1
    // ============================================================

    auto* canvas_nlead1 =
        PlotSupporter::MakeCanvas1D("canvas_nlead1", 2, 2);

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.035);

    for(size_t i = 0; i < 4; ++i){

        canvas_nlead1->cd(i + 1);

        PlotSupporter::SetHistStyle1D(hist_array[i][0]);

        hist_array[i][0]->Draw("HIST");

        if(fit_array[i][0]){
            fit_array[i][0]->Draw("SAME");
        }

        auto it = run_map.find(td.run_number_);

        latex.DrawLatex(
            0.17, 0.87,
            Form("Run %s", td.run_number_.c_str())
        );

        if(it != run_map.end()){
            latex.DrawLatex(
                0.17, 0.82,
                Form("Energy: %.1f GeV", it->second.energy)
            );

            latex.DrawLatex(
                0.17, 0.77,
                Form("W plate: %d", it->second.nw)
            );
        }

        latex.DrawLatex(
            0.17, 0.72,
            "n_{leading} = 1"
        );

        latex.DrawLatex(
            0.17, 0.67,
            Form(
                "Entries = %.0f",
                hist_array[i][0]->GetEntries()
            )
        );

        if(fit_array[i][0]){

            latex.DrawLatex(
                0.17, 0.62,
                Form(
                    "#mu = %.3f",
                    fit_array[i][0]->GetParameter(1)
                )
            );

            latex.DrawLatex(
                0.17, 0.57,
                Form(
                    "#sigma = %.3f",
                    fit_array[i][0]->GetParameter(2)
                )
            );
        }
    }

    canvas_nlead1->SaveAs(
        (save_dir / "difference_nleading1.png")
            .string()
            .c_str()
    );


    // ============================================================
    // n_leading >= 2
    // ============================================================

    auto* canvas_nlead_ge2 =
        PlotSupporter::MakeCanvas1D("canvas_nlead_ge2", 2, 2);

    for(size_t i = 0; i < 4; ++i){

        canvas_nlead_ge2->cd(i + 1);

        PlotSupporter::SetHistStyle1D(hist_array[i][1]);

        hist_array[i][1]->Draw("HIST");

        if(fit_array[i][1]){
            fit_array[i][1]->Draw("SAME");
        }

        auto it = run_map.find(td.run_number_);

        latex.DrawLatex(
            0.17, 0.87,
            Form("Run %s", td.run_number_.c_str())
        );

        if(it != run_map.end()){

            latex.DrawLatex(
                0.17, 0.82,
                Form("Energy: %.1f GeV", it->second.energy)
            );

            latex.DrawLatex(
                0.17, 0.77,
                Form("W plate: %d", it->second.nw)
            );
        }

        latex.DrawLatex(
            0.17, 0.72,
            "n_{leading} #geq 2"
        );

        latex.DrawLatex(
            0.17, 0.67,
            Form(
                "Entries = %.0f",
                hist_array[i][1]->GetEntries()
            )
        );

        if(fit_array[i][1]){

            latex.DrawLatex(
                0.17, 0.62,
                Form(
                    "#mu = %.3f",
                    fit_array[i][1]->GetParameter(1)
                )
            );

            latex.DrawLatex(
                0.17, 0.57,
                Form(
                    "#sigma = %.3f",
                    fit_array[i][1]->GetParameter(2)
                )
            );
        }
    }

    canvas_nlead_ge2->SaveAs(
        (save_dir / "difference_nleading_ge2.png")
            .string()
            .c_str()
    );


    return 0;
}