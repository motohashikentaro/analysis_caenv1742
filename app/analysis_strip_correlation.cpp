#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"

#include <filesystem>

#include <TH1D.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TF1.h>

int main(int argc, char* argv[]){

    ThroughEventReader tr(argv[1]);

    ThroughEventData& td = tr.GetThroughEventData();

    TH1D* front_x_hist = new TH1D("front_x_hist", "Front X Position Histogram", 32, -2, 2);
    TH1D* front_y_hist = new TH1D("front_y_hist", "Front Y Position Histogram", 32, -2, 2);
    TH1D* back_x_hist = new TH1D("back_x_hist", "Back X Position Histogram", 32, -2, 2);
    TH1D* back_y_hist = new TH1D("back_y_hist", "Back Y Position Histogram", 32, -2, 2);

    TH1D* difference_x_hist = new TH1D("difference_x_hist", "Difference X Position Histogram", 32, -2, 2);
    TH1D* difference_y_hist = new TH1D("difference_y_hist", "Difference Y Position Histogram", 32, -2, 2);

    TH1D* difference_x_hist_lemm1 = new TH1D("difference_x_hist_lemm1", "Difference X Position Histogram", 32, -2, 2);
    TH1D* difference_y_hist_lemm1 = new TH1D("difference_y_hist_lemm1", "Difference Y Position Histogram", 32, -2, 2);

    TH1D* difference_x_hist_lemm05 = new TH1D("difference_x_hist_lemm05", "Difference X Position Histogram", 32, -2, 2);
    TH1D* difference_y_hist_lemm05 = new TH1D("difference_y_hist_lemm05", "Difference Y Position Histogram", 32, -2, 2);

    TH1D* difference_x_hist_lem0 = new TH1D("difference_x_hist_lem0", "Difference X Position Histogram", 32, -2, 2);
    TH1D* difference_y_hist_lem0 = new TH1D("difference_y_hist_lem0", "Difference Y Position Histogram", 32, -2, 2);

    TH1D* difference_x_hist_lem05 = new TH1D("difference_x_hist_lem05", "Difference X Position Histogram", 32, -2, 2);
    TH1D* difference_y_hist_lem05 = new TH1D("difference_y_hist_lem05", "Difference Y Position Histogram", 32, -2, 2);

    TH1D* difference_x_hist_lem1 = new TH1D("difference_x_hist_lem1", "Difference X Position Histogram", 32, -2, 2);
    TH1D* difference_y_hist_lem1 = new TH1D("difference_y_hist_lem1", "Difference Y Position Histogram", 32, -2, 2);

    TH1D* limity_difference_y_limm1 = new TH1D("limity_difference_y_limm1", "Difference Y Position Histogram", 32, -2, 2);
    TH1D* limity_difference_y_limm05 = new TH1D("limity_difference_y_limm05", "Difference Y Position Histogram", 32, -2, 2);
    TH1D* limity_difference_y_lem0 = new TH1D("limity_difference_y_lem0", "Difference Y Position Histogram", 32, -2, 2);
    TH1D* limity_difference_y_lem05 = new TH1D("limity_difference_y_lem05", "Difference Y Position Histogram", 32, -2, 2);
    TH1D* limity_difference_y_lem1 = new TH1D("limity_difference_y_lem1", "Difference Y Position Histogram", 32, -2, 2);

    TH1D* double_limity_difference_x_limm1_0 = new TH1D("double_limity_difference_x_limm1_0", "Difference X Position Histogram", 32, -2, 2);
    TH1D* double_limity_difference_y_limm05_0 = new TH1D("double_limity_difference_y_limm05_0", "Difference Y Position Histogram", 32, -2, 2);

    TH1D* double_limity_difference_x_lemm1_1 = new TH1D("double_limity_difference_x_lemm1_1", "Difference X Position Histogram", 32, -2, 2);
    TH1D* double_limity_difference_y_lemm1_1 = new TH1D("double_limity_difference_y_lemm1_1", "Difference Y Position Histogram", 32, -2, 2);

    td.tree_->Draw("strip_position_front_x>>front_x_hist", "", "");
    td.tree_->Draw("strip_position_front_y>>front_y_hist", "", "");
    td.tree_->Draw("strip_position_back_x>>back_x_hist", "", "");
    td.tree_->Draw("strip_position_back_y>>back_y_hist", "", "");

    td.tree_->Draw("strip_position_front_x - strip_position_back_x>>difference_x_hist", "", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>difference_y_hist", "", "");

    td.tree_->Draw("strip_position_front_x - strip_position_back_x>>difference_x_hist_lemm1", "strip_position_front_x >= -1", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>difference_y_hist_lemm1", "strip_position_front_x >= -1", "");

    td.tree_->Draw("strip_position_front_x - strip_position_back_x>>difference_x_hist_lemm05", "strip_position_front_x >= -0.5", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>difference_y_hist_lemm05", "strip_position_front_x >= -0.5", "");

    td.tree_->Draw("strip_position_front_x - strip_position_back_x>>difference_x_hist_lem0", "strip_position_front_x >= 0", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>difference_y_hist_lem0", "strip_position_front_x >= 0", "");

    td.tree_->Draw("strip_position_front_x - strip_position_back_x>>difference_x_hist_lem05", "strip_position_front_x >= 0.5", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>difference_y_hist_lem05", "strip_position_front_x >= 0.5", "");

    td.tree_->Draw("strip_position_front_x - strip_position_back_x>>difference_x_hist_lem1", "strip_position_front_x >= 1", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>difference_y_hist_lem1", "strip_position_front_x >= 1", "");

    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>limity_difference_y_limm1", "strip_position_front_y >= -1", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>limity_difference_y_limm05", "strip_position_front_y >= -0.5", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>limity_difference_y_lem0", "strip_position_front_y >= 0", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>limity_difference_y_lem05", "strip_position_front_y >= 0.5", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>limity_difference_y_lem1", "strip_position_front_y >= 1", "");

    td.tree_->Draw("strip_position_front_x - strip_position_back_x>>double_limity_difference_x_limm1_0", "strip_position_front_x >= -1 && strip_position_front_x <= 0", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>double_limity_difference_y_limm05_0", "strip_position_front_y >= -0.5 && strip_position_front_y <= 0", "");

    td.tree_->Draw("strip_position_front_x - strip_position_back_x>>double_limity_difference_x_lemm1_1", "strip_position_front_x >= -1 && strip_position_front_x <= 1", "");
    td.tree_->Draw("strip_position_front_y - strip_position_back_y>>double_limity_difference_y_lemm1_1", "strip_position_front_y >= -1 && strip_position_front_y <= 1", "");

    TF1* fit = new TF1("fit", "gaus", -1.2, 0.3);
    double_limity_difference_x_lemm1_1->Fit(fit, "R");
    double mean  = fit->GetParameter(1);
    double sigma = fit->GetParameter(2);

    std::cout << "mean  = " << mean << std::endl;
    std::cout << "sigma = " << sigma << std::endl;

    PlotSupporter ps(td);

    std::filesystem::path output_path = ps.MakeSaveDir() / "position_histograms.root";

    TCanvas* canvas1d = PlotSupporter::MakeCanvas1D("position_histograms", 1, 1);
    canvas1d->cd();
    double_limity_difference_x_lemm1_1->Draw();
    fit->Draw("same");
    canvas1d->SaveAs((output_path.parent_path() / "double_limity_difference_x_lemm1_1.png").string().c_str());


    TFile* output_file = new TFile(output_path.string().c_str(), "RECREATE");
    output_file->cd();
    front_x_hist->Write();
    front_y_hist->Write();
    back_x_hist->Write();
    back_y_hist->Write();
    difference_x_hist->Write();
    difference_y_hist->Write();
    difference_x_hist_lemm05->Write();
    difference_y_hist_lemm05->Write();
    difference_x_hist_lemm1->Write();
    difference_y_hist_lemm1->Write();
    difference_x_hist_lem0->Write();
    difference_y_hist_lem0->Write();
    difference_x_hist_lem05->Write();
    difference_y_hist_lem05->Write();
    difference_x_hist_lem1->Write();
    difference_y_hist_lem1->Write();
    limity_difference_y_limm1->Write();
    limity_difference_y_limm05->Write();
    limity_difference_y_lem0->Write();
    limity_difference_y_lem05->Write();
    limity_difference_y_lem1->Write();
    double_limity_difference_x_limm1_0->Write();
    double_limity_difference_y_limm05_0->Write();
    output_file->Close();

    delete front_x_hist;
    delete front_y_hist;
    delete back_x_hist;
    delete back_y_hist;
    delete difference_x_hist;
    delete difference_y_hist;
    delete difference_x_hist_lemm05;
    delete difference_y_hist_lemm05;
    delete difference_x_hist_lemm1;
    delete difference_y_hist_lemm1;
    delete difference_x_hist_lem0;
    delete difference_y_hist_lem0;
    delete difference_x_hist_lem05;
    delete difference_y_hist_lem05;
    delete difference_x_hist_lem1;
    delete difference_y_hist_lem1;
    delete limity_difference_y_limm1;
    delete limity_difference_y_limm05;
    delete limity_difference_y_lem0;
    delete limity_difference_y_lem05;
    delete limity_difference_y_lem1;
    delete double_limity_difference_x_limm1_0;
    delete double_limity_difference_y_limm05_0;
    delete output_file;

    return 0;
}