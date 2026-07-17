// #include "./../include/feature_analyzer.h"
// #include "./../include/hit_selection.h"
// #include "./../include/hit_analyzer.h"

// #include <filesystem>

// #include <TCanvas.h>
// #include <TH1D.h>
// #include <TH2D.h>
// #include <TColor.h>
// #include <TStyle.h>

// #include "./../include/hit_analyzer.h"

// #include <filesystem>

// #include <TCanvas.h>
// #include <TH1D.h>
// #include <TH2D.h>
// #include <TColor.h>
// #include <TStyle.h>

// HitAnalyzer::HitAnalyzer(char* input_path){
//     hd_.file_ = TFile::Open(input_path);

//     std::filesystem::path path(input_path);
//     hd_.filename_ = path.stem().string();
//     hd_.run_number_ = hd_.filename_.substr(4);

//     for(size_t ithres=0; ithres<nthres; ithres++){

//         hd_.trees_[ithres] =
//             (TTree*)hd_.file_->Get(
//                 ("tree_th" + std::to_string(thresholds[ithres])).c_str());

//         TTree* tree = hd_.trees_[ithres];

//         tree->SetBranchAddress("evt",       &hd_.ef_.evt);
//         tree->SetBranchAddress("board",     &hd_.ef_.board);
//         tree->SetBranchAddress("ch",        &hd_.ef_.ch);
//         tree->SetBranchAddress("pedestal",  &hd_.ef_.pedestal);
//         tree->SetBranchAddress("peak_adc",  &hd_.ef_.peak_adc);
//         tree->SetBranchAddress("peak_time", &hd_.ef_.peak_time);

//         tree->SetBranchAddress("raise_time", &hd_.ef_.raise_times[ithres]);
//         tree->SetBranchAddress("fall_time",  &hd_.ef_.fall_times[ithres]);
//         tree->SetBranchAddress("charge",     &hd_.ef_.charges[ithres]);
//         tree->SetBranchAddress("tot",        &hd_.ef_.tots[ithres]);

//         hd_.nentries_[ithres] = tree->GetEntries();
//     }
// }

// HitAnalyzer::~HitAnalyzer(){
//     if(hd_.file_){
//         hd_.file_->Close();
//         delete hd_.file_;
//     }
// }

// TH1D* HitAnalyzer::PeakDistro(){

//     constexpr size_t ithres = 1;

//     TH1D* hist = new TH1D(
//         Form("hist_peak_%p", this),
//         "Peak ADC Distro;ADC [ADC];Entries",
//         100, -500, 0);

//     hist->SetLineWidth(2);
//     hist->SetLineColor(thres_colors[ithres]);

//     hd_.trees_[ithres]->Draw(
//         Form("peak_adc>>hist_peak_%p", this),
//         "board==1 && ch==24",
//         "");

//     return hist;
// }

// std::array<TH1D*, nthres> HitAnalyzer::ChargeDistro(){

//     std::array<TH1D*, nthres> hists;

//     for(size_t ithres=0; ithres<nthres; ithres++){

//         hists[ithres] = new TH1D(
//             Form("hist_charge_%d", thresholds[ithres]),
//             Form("Charge Distro (th=%d);Charge;Entries",
//                  thresholds[ithres]),
//             100, 0, 300);

//         hists[ithres]->SetLineWidth(2);
//         hists[ithres]->SetLineColor(thres_colors[ithres]);

//         hd_.trees_[ithres]->Draw(
//             Form("charge>>hist_charge_%d", thresholds[ithres]),
//             "board==1 && ch==24",
//             "");
//     }

//     return hists;
// }

// TH1D* HitAnalyzer::PeaktimeDistro(){

//     constexpr size_t ithres = 1;

//     TH1D* hist = new TH1D(
//         Form("hist_ptime_%p", this),
//         "Peak Time Distro;Peak Time [sample];Entries",
//         100, 0, 1024);

//     hist->SetLineWidth(2);
//     hist->SetLineColor(thres_colors[ithres]);

//     hd_.trees_[ithres]->Draw(
//         Form("peak_time>>hist_ptime_%p", this),
//         "board==1 && ch==24",
//         "");

//     return hist;
// }

// std::array<TH2D*, nthres> HitAnalyzer::PeakVsCharge(){

//     std::array<TH2D*, nthres> hists;

//     gStyle->SetPalette(kViridis);

//     for(size_t ithres=0; ithres<nthres; ithres++){

//         hists[ithres] = new TH2D(
//             Form("hist_peak_vs_charge_th%d", thresholds[ithres]),
//             "Peak ADC VS Charge;-Peak ADC [ADC];Charge",
//             100, 0, 200,
//             100, 0, 300);

//         hd_.trees_[ithres]->Draw(
//             Form("charge:-peak_adc>>hist_peak_vs_charge_th%d",
//                  thresholds[ithres]),
//             "board==1 && ch==24",
//             "colz");
//     }

//     return hists;
// }

// TH1D* HitAnalyzer::TotDistro(){
//     constexpr size_t ithres = 1;
//     TH1D* hist = new TH1D(Form("hist_tot_%p", this), "ToT Distro;ToT;Entries", 100, 0, 30);
//     hist->SetLineWidth(2);
//     hist->SetLineColor(thres_colors[3]);
//     hist->SetFillColor(thres_colors[1]);
//     hd_.trees_[ithres]->Draw(Form("tot>>hist_tot_%p", this), "board==1 && ch==24", "");

//     return hist;
// }

#include "./../include/hit_analyzer.h"

#include <filesystem>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TColor.h>
#include <TStyle.h>
#include <TLatex.h>


HitAnalyzer::HitAnalyzer(char* input_path)
{
    hd_.file_ = TFile::Open(input_path);

    std::filesystem::path path(input_path);
    hd_.filename_ = path.stem().string();
    hd_.run_number_ = hd_.filename_.substr(4);


    constexpr size_t ithres = 1;   // threshold = 30 ADC

    hd_.tree_ =
        (TTree*)hd_.file_->Get(
            ("tree_th" + std::to_string(thresholds[ithres])).c_str()
        );


    hd_.tree_->SetBranchAddress("evt",       &hd_.ef_.evt);
    hd_.tree_->SetBranchAddress("board",     &hd_.ef_.board);
    hd_.tree_->SetBranchAddress("ch",        &hd_.ef_.ch);

    hd_.tree_->SetBranchAddress("pedestal",  &hd_.ef_.pedestal);

    hd_.tree_->SetBranchAddress("peak_adc",  &hd_.ef_.peak_adc);
    hd_.tree_->SetBranchAddress("peak_time", &hd_.ef_.peak_time);

    hd_.tree_->SetBranchAddress("raise_time",
                                &hd_.ef_.raise_times[ithres]);

    hd_.tree_->SetBranchAddress("fall_time",
                                &hd_.ef_.fall_times[ithres]);

    hd_.tree_->SetBranchAddress("charge",
                                &hd_.ef_.charges[ithres]);

    hd_.tree_->SetBranchAddress("tot",
                                &hd_.ef_.tots[ithres]);

    hd_.nentries_ = hd_.tree_->GetEntries();
}


HitAnalyzer::~HitAnalyzer()
{
    if(hd_.file_){
        hd_.file_->Close();
        delete hd_.file_;
    }
}


void HitAnalyzer::DrawHistInfo(TH1* hist,
                               int ch,
                               double x,
                               double y)
{
    TLatex latex;

    latex.SetNDC();
    latex.SetTextFont(42);
    latex.SetTextSize(0.035);
    latex.SetTextAlign(13);

    constexpr double dy = 0.055;


    latex.DrawLatex(
        x, y, "Board : 1"
    );

    latex.DrawLatex(
        x, y-dy,
        Form("Channel : %d", ch)
    );

    latex.DrawLatex(
        x, y-2*dy,
        "Threshold : 30 ADC"
    );

    latex.DrawLatex(
        x, y-3*dy,
        Form("Entries : %.0f", hist->GetEntries())
    );
}



TH1D* HitAnalyzer::PeakDistro(int ch)
{
    TH1D* hist = new TH1D(
        Form("hist_peak_%p_ch%d", this, ch),
        "Peak ADC Distro;ADC [ADC];Entries",
        100, -400, 0
    );


    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(thres_colors[0]);
    hist->SetStats(0);


    hd_.tree_->Draw(
        Form("peak_adc>>hist_peak_%p_ch%d",
             this, ch),
        Form("board==1 && ch==%d", ch),
        ""
    );


    return hist;
}



TH1D* HitAnalyzer::ChargeDistro(int ch)
{
    TH1D* hist = new TH1D(
        Form("hist_charge_%p_ch%d", this, ch),
        "Charge Distro (Threshold = 30 ADC);Charge;Entries",
        100, 0, 1000
    );


    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(thres_colors[0]);
    hist->SetStats(0);


    hd_.tree_->Draw(
        Form("charge>>hist_charge_%p_ch%d",
             this, ch),
        Form("board==1 && ch==%d", ch),
        ""
    );


    return hist;
}



TH1D* HitAnalyzer::PeaktimeDistro(int ch)
{
    TH1D* hist = new TH1D(
        Form("hist_ptime_%p_ch%d", this, ch),
        "Peak Time Distro;Peak Time [sample];Entries",
        100, 0, 1024
    );


    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(thres_colors[0]);
    hist->SetStats(0);


    hd_.tree_->Draw(
        Form("peak_time>>hist_ptime_%p_ch%d",
             this, ch),
        Form("board==1 && ch==%d", ch),
        ""
    );


    return hist;
}



TH2D* HitAnalyzer::PeakVsCharge(int ch)
{
    TH2D* hist = new TH2D(
        Form("hist_peak_vs_charge_%p_ch%d",
             this, ch),
        "Peak ADC vs Charge;-Peak ADC [ADC];Charge",
        200, 0, 400,
        200, 0, 1200
    );


    gStyle->SetPalette(kViridis);


    hd_.tree_->Draw(
        Form("charge:-peak_adc>>hist_peak_vs_charge_%p_ch%d",
             this, ch),
        Form("board==1 && ch==%d", ch),
        "colz"
    );


    return hist;
}



TH1D* HitAnalyzer::TotDistro(int ch)
{
    TH1D* hist = new TH1D(
        Form("hist_tot_%p_ch%d",
             this, ch),
        "ToT Distro (Threshold = 30 ADC);ToT;Entries",
        90, 0, 15
    );


    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(thres_colors[0]);
    hist->SetStats(0);


    hd_.tree_->Draw(
        Form("tot>>hist_tot_%p_ch%d",
             this, ch),
        Form("board==1   && ch==%d", ch),
        ""
    );


    return hist;
}