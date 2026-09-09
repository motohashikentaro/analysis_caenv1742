#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/run_map.h"
#include "./../include/alignment_data.h"

#include <iostream>
#include <filesystem>
#include <string>
#include <array>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <limits>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLatex.h>


int main(int argc, char* argv[]){


    ThroughEventReader tr(argv[1]);  // Create an instance of ThroughEventReader with the input ROOT file path
    ThroughEventData& td = tr.GetThroughEventData();  // Get the ThroughEventData object

    std::array<std::string, 2> axes = {"x", "y"};
    std::array<std::string, 2> sides = {"front", "back"};

    std::array<TH1D*, 4> hist_array;
    std::array<TF1*, 4> fit_array;

    constexpr double dut_half_size = 1.0;

    NewAlignmentDutData ad;

    size_t i=0;

    for(size_t axis_idx=0; axis_idx<axes.size(); ++axis_idx){
        const auto& axis = axes[axis_idx];
        for(size_t side_idx=0; side_idx<sides.size(); ++side_idx){
            const auto& side = sides[side_idx];
            
            const std::string hist_name = "hist_" + side + "_" + axis;
            const std::string hist_title = ";" + axis + "_{reco} - " + axis + "_{extrapolated} [mm];Entries";
            const std::string expression = "dut_position_" + side + "_" + axis + " - extrapolated_" + side + "_" + axis;
            const std::string fit_name = "fit_" + side + "_" + axis;

            double alignment_x;
            double alignment_y;

            if(side == "front"){
                alignment_x = ad.front_x;
                alignment_y = ad.front_y;
            } else {
                alignment_x = ad.back_x;
                alignment_y = ad.back_y;
            }

            const double xmin = -dut_half_size - alignment_x;
            const double xmax = dut_half_size - alignment_x;
            const double ymin = -dut_half_size - alignment_y;
            const double ymax = dut_half_size - alignment_y;

            const std::string cut = Form(
                "extrapolated_%s_x >= %.6f && "
                "extrapolated_%s_x <= %.6f && "
                "extrapolated_%s_y >= %.6f && "
                "extrapolated_%s_y <= %.6f",
                side.c_str(), xmin,
                side.c_str(), xmax,
                side.c_str(), ymin,
                side.c_str(), ymax
            );

            hist_array[i] = new TH1D(hist_name.c_str(), hist_title.c_str(), 32, -3, 3);
            
            td.tree_->Draw((expression + ">>" + hist_name).c_str(), cut.c_str(), "");

            const int max_bin = hist_array[i]->GetMaximumBin();
            const double peak_position = hist_array[i]->GetBinCenter(max_bin);

            double fit_min = peak_position - 2.0;
            double fit_max = peak_position + 2.0;

            fit_array[i] = new TF1(fit_name.c_str(), 
            "[0]*exp(-0.5*((x-[1])/[2])^2) + [3]*exp(-0.5*((x-[1])/[4])^2)", fit_min, fit_max);

            fit_array[i]->SetParNames("Peak_{single}", "Mean", "#sigma_{single}", "Peak_{shower}", "#sigma_{shower}");

            const double max_value = hist_array[i]->GetMaximum();

            fit_array[i]->SetParameters(
                max_value * 0.7,
                peak_position,
                0.2,
                max_value * 0.3,
                1.0
            );

            hist_array[i]->Fit(fit_name.c_str(), "R");

            i++;
        }
    }

    gStyle->SetOptStat(0);
    TLatex latex;
    latex.SetNDC();

    PlotSupporter ps(td);
    std::filesystem::path save_dir = ps.MakeSaveDir();
    auto* canvas = PlotSupporter::MakeCanvas1D("canvas", 2, 2);
    for(size_t j=0; j<4; ++j){
        canvas->cd(j+1);
        PlotSupporter::SetHistStyle1D(hist_array[j]);
        hist_array[j]->Draw("HIST");
        fit_array[j]->Draw("SAME");

        double amp1   = fit_array[j]->GetParameter(0);
        double mean = fit_array[j]->GetParameter(1);
        double sigma1 = fit_array[j]->GetParameter(2);
        double amp2   = fit_array[j]->GetParameter(3);
        double sigma2 = fit_array[j]->GetParameter(4);

        auto it = run_map.find(td.run_number_);
        latex.DrawLatex(0.17, 0.87, Form("Run %s", td.run_number_.c_str()));
        if(it != run_map.end()) latex.DrawLatex(0.17, 0.82, Form("Energy: %.1f GeV", it->second.energy));
        if(it != run_map.end()) latex.DrawLatex(0.17, 0.77, Form("W plate: %d", it->second.nw));
        latex.DrawLatex(0.17, 0.72, Form("#mu = %.2f", mean));
        latex.DrawLatex(0.17, 0.67, Form("#sigma_{single} = %.2f", sigma1));
        latex.DrawLatex(0.17, 0.62, Form("#sigma_{shower} = %.2f", sigma2));
    }

    canvas->SaveAs((save_dir / "difference_histograms_limit.png").string().c_str());

    std::ofstream ofs(save_dir / "fit_results_limit.txt");

    if(!ofs){
        std::cerr << "Failed to open fit_results_limit.txt" << std::endl;
        return 1;
    }

    ofs << std::fixed << std::setprecision(6);

    for(size_t j = 0; j < 4; ++j){

        // Fit parameters
        const double peak_single  = fit_array[j]->GetParameter(0);
        const double mean         = fit_array[j]->GetParameter(1);
        const double sigma_single = fit_array[j]->GetParameter(2);
        const double peak_shower  = fit_array[j]->GetParameter(3);
        const double sigma_shower = fit_array[j]->GetParameter(4);

        // Parameter errors
        const double peak_single_err  = fit_array[j]->GetParError(0);
        const double mean_err         = fit_array[j]->GetParError(1);
        const double sigma_single_err = fit_array[j]->GetParError(2);
        const double peak_shower_err  = fit_array[j]->GetParError(3);
        const double sigma_shower_err = fit_array[j]->GetParError(4);

        // sqrt(sigma_shower^2 - sigma_single^2)
        double sigma_difference =
            std::numeric_limits<double>::quiet_NaN();

        double sigma_difference_err =
            std::numeric_limits<double>::quiet_NaN();

        const double sigma2_diff =
            sigma_shower * sigma_shower
            - sigma_single * sigma_single;

        if(sigma2_diff > 0.0){
            sigma_difference = std::sqrt(sigma2_diff);

            // Error propagation assuming sigma_single and sigma_shower are independent
            sigma_difference_err =
                std::sqrt(
                    sigma_shower * sigma_shower
                        * sigma_shower_err * sigma_shower_err
                    +
                    sigma_single * sigma_single
                        * sigma_single_err * sigma_single_err
                ) / sigma_difference;
        }

        // Chi-square
        const double chi2 = fit_array[j]->GetChisquare();
        const int ndf = fit_array[j]->GetNDF();

        const double chi2_ndf =
            (ndf > 0)
            ? chi2 / static_cast<double>(ndf)
            : std::numeric_limits<double>::quiet_NaN();

        // j = 0 front_x
        // j = 1 back_x
        // j = 2 front_y
        // j = 3 back_y
        const size_t axis_idx = j / 2;
        const size_t side_idx = j % 2;

        ofs << "[" << sides[side_idx]
            << "_" << axes[axis_idx] << "]\n";

        ofs << "Peak_single      = "
            << peak_single << " +/- "
            << peak_single_err << "\n";

        ofs << "Mean             = "
            << mean << " +/- "
            << mean_err << " mm\n";

        ofs << "Sigma_single     = "
            << sigma_single << " +/- "
            << sigma_single_err << " mm\n";

        ofs << "Peak_shower      = "
            << peak_shower << " +/- "
            << peak_shower_err << "\n";

        ofs << "Sigma_shower     = "
            << sigma_shower << " +/- "
            << sigma_shower_err << " mm\n";

        ofs << "Sigma_difference = "
            << sigma_difference << " +/- "
            << sigma_difference_err << " mm\n";

        ofs << "Chi2             = "
            << chi2 << "\n";

        ofs << "NDF              = "
            << ndf << "\n";

        ofs << "Chi2/NDF         = "
            << chi2_ndf << "\n";

        ofs << "\n";
    }

    ofs.close();

    return 0;
}