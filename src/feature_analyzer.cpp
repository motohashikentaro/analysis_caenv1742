#include "./../include/feature_analyzer.h"
#include "./../include/hit_selection.h"

#include <filesystem>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TColor.h>
#include <TStyle.h>

FeatureAnalyzer::FeatureAnalyzer(char* input_path){
    fd_.file_ = TFile::Open(input_path);

    std::filesystem::path path(input_path);
    fd_.filename_ = path.stem().string();

    fd_.run_rumber_ = fd_.filename_.substr(4);

    fd_.tree_ = (TTree*)fd_.file_->Get("tree");

    fd_.tree_->SetBranchAddress("evt", &fd_.ef_.evt);
    
    fd_.tree_->SetBranchAddress("board", &fd_.ef_.board);
    fd_.tree_->SetBranchAddress("ch", &fd_.ef_.ch);

    fd_.tree_->SetBranchAddress("pedestal", &fd_.ef_.pedestal);

    fd_.tree_->SetBranchAddress("peak_adc", &fd_.ef_.peak_adc);
    fd_.tree_->SetBranchAddress("peak_time", &fd_.ef_.peak_time);

    for(size_t ithres=0; ithres<nthres; ithres++){
        fd_.tree_->SetBranchAddress(("raise_time_th" + std::to_string(thresholds[ithres])).c_str(), &fd_.ef_.raise_times[ithres]);
        fd_.tree_->SetBranchAddress(("fall_time_th" + std::to_string(thresholds[ithres])).c_str(), &fd_.ef_.fall_times[ithres]);
        fd_.tree_->SetBranchAddress(("charge_th" + std::to_string(thresholds[ithres])).c_str(), &fd_.ef_.charges[ithres]);
        fd_.tree_->SetBranchAddress(("tot_th" + std::to_string(thresholds[ithres])).c_str(), &fd_.ef_.tots[ithres]);
    }

    fd_.nentries_ = fd_.tree_->GetEntries();
}

FeatureAnalyzer::~FeatureAnalyzer(){
    if(fd_.file_){
        fd_.file_->Close();
        delete fd_.file_;
    }
}

// TH1D* FeatureAnalyzer::PeakDistro(){
//     TH1D* hist = new TH1D(Form("hist_peak_%p", this), "Peak ADC Distro;ADC [ADC];Entries", 100, -500, 0);
//     hist->SetLineWidth(2);
//     hist->SetLineColor(thres_colors[3]);
//     fd_.tree_->Draw(Form("peak_adc>>hist_peak_%p", this), "board==1 && ch==24", "");

//     return hist;
// }

// std::array<TH1D*, nthres> FeatureAnalyzer::ChargeDistro(){
//     std::array<TH1D*, nthres> hists;
//     for(size_t ithres=0; ithres<nthres; ithres++){
//         hists[ithres] = new TH1D(Form("hist_charge_%d", thresholds[ithres]), Form("Charge Distro (thres=%d);Charge;Entries", thresholds[ithres]), 100, 0, 300);
//         hists[ithres]->SetLineWidth(2);
//         hists[ithres]->SetLineColor(thres_colors[ithres]);        
//         fd_.tree_->Draw(Form("charge_th%d>>hist_charge_%d", thresholds[ithres], thresholds[ithres]), "board==1 && ch==24", "");

//     }

//     return hists;
// }

// TH1D* FeatureAnalyzer::PeaktimeDistro(){
//     TH1D* hist = new TH1D(Form("hist_ptime_%p", this), "Peak Time Distro;Peak Time[sample];Entries", 100, 0, 1024);
//     hist->SetLineWidth(2);
//     hist->SetLineColor(thres_colors[3]);
//     fd_.tree_->Draw(Form("peak_time>>hist_ptime_%p", this), "board==1 && ch==24", "");

//     return hist;
// }

// std::array<TH2D*, nthres> FeatureAnalyzer::PeakVsCharge(){
//     std::array<TH2D*, nthres> hists;
//     for(size_t ithres=0; ithres<nthres; ithres++){
//         hists[ithres] = new TH2D(Form("hist_peak_vs_charge_th%d", thresholds[ithres]), "Peak ADC VS Charge;-Peak ADC [ADC];Charge", 100, 0, 200, 100, 0, 300);
//         gStyle->SetPalette(kViridis);
//         fd_.tree_->Draw(Form("charge_th%d:-peak_adc>>hist_peak_vs_charge_th%d", thresholds[ithres], thresholds[ithres]), "board==1 && ch==24", "colz");
//     }

//     return hists;
// }

// TH1D* FeatureAnalyzer::TotDistro(){
//     TH1D* hist = new TH1D(Form("hist_tot_%p", this), "ToT Distro;ToT;Entries", 100, 0, 30);
//     hist->SetLineWidth(2);
//     hist->SetLineColor(thres_colors[3]);
//     hist->SetFillColor(thres_colors[1]);
//     fd_.tree_->Draw(Form("tot_th30>>hist_tot_%p", this), "board==1 && ch==24", "");

//     return hist;
// }

void FeatureAnalyzer::DrawHistInfo(TH1* hist,
                                   int board,
                                   int ch,
                                   int threshold,
                                   double x,
                                   double y)
{
    TLatex latex;
    latex.SetNDC();
    latex.SetTextFont(42);
    latex.SetTextSize(0.035);
    latex.SetTextAlign(13);

    const double dy = 0.055;

    latex.DrawLatex(x, y - 0 * dy,
                    Form("Board : %d", board));

    latex.DrawLatex(x, y - 1 * dy,
                    Form("Channel : %d", ch));

    latex.DrawLatex(x, y - 2 * dy,
                    Form("Threshold : %d ADC", threshold));

    latex.DrawLatex(x, y - 3 * dy,
                    Form("Entries : %.0f", hist->GetEntries()));

    // latex.DrawLatex(x, y - 4 * dy,
    //                 Form("Mean : %.2f", hist->GetMean()));

    // latex.DrawLatex(x, y - 5 * dy,
    //                 Form("RMS : %.2f", hist->GetRMS()));
}

TH1D* FeatureAnalyzer::PeakDistro(int ch){
    TH1D* hist = new TH1D(
        Form("hist_peak_%p", this),
        "Peak ADC Distro;ADC [ADC];Entries",
        100, -400, 0
    );

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(thres_colors[0]);
    hist->SetStats(0);

    fd_.tree_->Draw(
        Form("peak_adc>>hist_peak_%p", this),
        Form("board==1 && ch==%d", ch),
        ""
    );

    return hist;
}

TH1D* FeatureAnalyzer::ChargeDistro(int ch){
    TH1D* hist = new TH1D(
        Form("hist_charge_%p", this),
        "Charge Distro (Threshold = 30 ADC);Charge;Entries",
        100, 1, 1000
    );

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(thres_colors[0]);
    hist->SetStats(0);

    fd_.tree_->Draw(
        Form("charge_th30>>hist_charge_%p", this),
        Form("board==1 && ch==%d", ch),
        ""
    );

    return hist;
}

TH1D* FeatureAnalyzer::PeaktimeDistro(int ch){
    TH1D* hist = new TH1D(
        Form("hist_ptime_%p", this),
        "Peak Time Distro;Peak Time [sample];Entries",
        100, 0, 1024
    );

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(thres_colors[0]);
    hist->SetStats(0);

    fd_.tree_->Draw(
        Form("peak_time>>hist_ptime_%p", this),
        Form("board==1 && ch==%d", ch),
        ""
    );

    return hist;
}

TH2D* FeatureAnalyzer::PeakVsCharge(int ch){
    TH2D* hist = new TH2D(
        Form("hist_peak_vs_charge_%p", this),
        "Peak ADC vs Charge;-Peak ADC [ADC];Charge",
        200, 0, 400,
        200, 0, 1200
    );

    gStyle->SetPalette(kViridis);

    fd_.tree_->Draw(
        Form("charge_th30:-peak_adc>>hist_peak_vs_charge_%p", this),
        Form("board==1 && ch==%d", ch),
        "colz"
    );

    return hist;
}

TH1D* FeatureAnalyzer::TotDistro(int ch){
    TH1D* hist = new TH1D(
        Form("hist_tot_%p", this),
        "ToT Distro (Threshold = 30 ADC);ToT;Entries",
        90, 1, 15
    );

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(thres_colors[0]);
    hist->SetStats(0);

    fd_.tree_->Draw(
        Form("tot_th30>>hist_tot_%p", this),
        Form("board==1 && ch==%d", ch),
        ""
    );

    return hist;
}