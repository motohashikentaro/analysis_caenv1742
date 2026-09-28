#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/run_map.h"

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
#include <TFitResult.h>
#include <TFitResultPtr.h>


int main(int argc, char* argv[]){


    ThroughEventReader tr(argv[1]);  // Create an instance of ThroughEventReader with the input ROOT file path
    ThroughEventData& td = tr.GetThroughEventData();  // Get the ThroughEventData object

    std::array<std::string, 2> axes = {"x", "y"};
    std::array<std::string, 2> sides = {"front", "back"};

    std::array<TH1D*, 4> hist_array;
    std::array<TF1*, 4> fit_array;
    std::array<TFitResultPtr, 4> fit_result_array;

    size_t i=0;

    for(const auto& axis : axes){
        for(const auto& side : sides){
            
            const std::string hist_name = "hist_" + side + "_" + axis;
            const std::string hist_title = ";" + side + " " + axis + "_{reco} - " + axis + "_{extrapolated} [mm];Entries";
            const std::string expression = "dut_position_" + side + "_" + axis + " - extrapolated_" + side + "_" + axis;
            const std::string fit_name = "fit_" + side + "_" + axis;

            hist_array[i] = new TH1D(hist_name.c_str(), hist_title.c_str(), 32, -3, 3);
            
            td.tree_->Draw((expression + ">>" + hist_name).c_str(), "", "");

            const int max_bin = hist_array[i]->GetMaximumBin();
            const double peak_position = hist_array[i]->GetBinCenter(max_bin);

            double fit_min, fit_max;

            if(td.run_number_ == "05434-05438"){
                fit_min = peak_position - 0.7;
                fit_max = peak_position + 0.7;
            }else if(td.run_number_ == "05265"){
                if(side == "back" && axis == "x"){
                    fit_min = peak_position - 2.0;
                    fit_max = peak_position + 2.0;
                }else{
                fit_min = peak_position - 1.0;
                fit_max = peak_position + 1.0;
                }
            }else{
                fit_min = peak_position - 0.5;
                fit_max = peak_position + 0.5;
            }


            fit_array[i] = new TF1(fit_name.c_str(), 
            "[0]*exp(-0.5*((x-[1])/[2])^2)", fit_min, fit_max);

            fit_array[i]->SetParNames("Peak", "Mean", "#sigma");
            
            if(td.run_number_ == "05265" && side == "back" && axis == "x"){
                fit_array[i]->SetParLimits(2, 0.01, 2.0);
            }else{
                fit_array[i]->SetParLimits(2, 0.01, 1.0);
            }

            const double max_value = hist_array[i]->GetMaximum();

            fit_array[i]->SetParameters(
                max_value,
                peak_position,
                0.5
            );

            fit_result_array[i] = hist_array[i]->Fit(fit_name.c_str(), "RS");

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
        double mean1 = fit_array[j]->GetParameter(1);
        double sigma1 = fit_array[j]->GetParameter(2);

        auto it = run_map.find(td.run_number_);
        latex.DrawLatex(0.15, 0.89, Form("Work in Progress"));
        latex.DrawLatex(0.15, 0.84, Form("Run %s", td.run_number_.c_str()));
        if(it != run_map.end()) latex.DrawLatex(0.15, 0.79, Form("Energy: %.1f GeV", it->second.energy));
        if(it != run_map.end()) latex.DrawLatex(0.15, 0.74, Form("W plate: %d", it->second.nw));
        latex.DrawLatex(0.15, 0.69, Form("#mu = %.2f", mean1));
        latex.DrawLatex(0.15, 0.64, Form("#sigma = %.2f", sigma1));
    }

    canvas->SaveAs((save_dir / "slide_difference_histograms_single_gaussian.png").string().c_str());

    std::ofstream ofs(save_dir / "slide_fit_results_single_gaussian.txt");

    if(!ofs){
        std::cerr << "Failed to open fit_results.txt" << std::endl;
        return 1;
    }

    ofs << std::fixed << std::setprecision(6);

    ofs << "Run_number = " << td.run_number_ << "\n\n";

    for(size_t j = 0; j < 4; ++j){

        // Fit parameters
        const double peak_single  = fit_array[j]->GetParameter(0);
        const double mean1         = fit_array[j]->GetParameter(1);
        const double sigma_single = fit_array[j]->GetParameter(2);
        const auto fit_status = fit_result_array[j]->Status();
        const int cov_matrix_status = fit_result_array[j]->CovMatrixStatus();
        const bool fit_valid = fit_result_array[j]->IsValid();

        // Parameter errors
        const double peak_single_err  = fit_array[j]->GetParError(0);
        const double mean1_err         = fit_array[j]->GetParError(1);
        const double sigma_single_err = fit_array[j]->GetParError(2);


        // Chi-square
        const double chi2 = fit_array[j]->GetChisquare();
        const int ndf = fit_array[j]->GetNDF();

        const double chi2_ndf =
            (ndf > 0)
            ? chi2 / static_cast<double>(ndf)
            : std::numeric_limits<double>::quiet_NaN();

        const size_t axis_idx = j / 2;
        const size_t side_idx = j % 2;

        ofs << "[" << sides[side_idx]
            << "_" << axes[axis_idx] << "]\n";

        ofs << "Peak_single      = "
            << peak_single << " +/- "
            << peak_single_err << "\n";



        ofs << "Mean_single      = "
            << mean1 << " +/- "
            << mean1_err << " mm\n";


        ofs << "Sigma_single     = "
            << sigma_single << " +/- "
            << sigma_single_err << " mm\n";


        ofs << "Chi2             = "
            << chi2 << "\n";

        ofs << "NDF              = "
            << ndf << "\n";

        ofs << "Chi2/NDF         = "
            << chi2_ndf << "\n";


        ofs << "Fit_status       = "
            << fit_status << "\n";

        ofs << "Fit_valid        = "
            << std::boolalpha
            << fit_valid << "\n";

        ofs << "CovMatrix_status = "
            << cov_matrix_status << "\n";

        ofs << "\n";
    }

    ofs.close();

    return 0;
}