#include "./../include/feature_analyzer.h"
#include "./../include/feature_reader.h"

#include <filesystem>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TColor.h>
#include <TStyle.h>

TH1D* FeatureAnalyzer::PeakDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_peak_%p", this), "Peak ADC Distro;ADC [ADC];Entries", 100, -400, 0);

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));
    hist->SetStats(0);

    fd_.tree_->Draw(Form("peak_adc>>hist_peak_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* FeatureAnalyzer::PeaktimeDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_ptime_%p", this), "Peak Time Distro;Peak Time [sample];Entries", 100, 0, 1024);

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));
    hist->SetStats(0);

    fd_.tree_->Draw(Form("peak_time>>hist_ptime_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* FeatureAnalyzer::ChargeDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_charge_%p", this), Form("Charge Distro (Threshold = %d ADC);Charge;Entries", target_threshold), 100, 1, 1000);

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));
    hist->SetStats(0);

    fd_.tree_->Draw(Form("charge_th%d>>hist_charge_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* FeatureAnalyzer::PeakVsCharge(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_peak_vs_charge_%p", this), Form("Peak ADC vs Charge (Threshold = %d ADC);-Peak ADC [ADC];Charge", target_threshold), 200, 0, 400, 200, 0, 1200);

    gStyle->SetPalette(kViridis);

    fd_.tree_->Draw(Form("charge_th%d:-peak_adc>>hist_peak_vs_charge_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "colz");

    return hist;
}

TH1D* FeatureAnalyzer::TotDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_tot_%p", this), Form("ToT Distro (Threshold = %d ADC);ToT;Entries", target_threshold), 90, 1, 15);

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));
    hist->SetStats(0);

    fd_.tree_->Draw(Form("tot_th%d>>hist_tot_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* FeatureAnalyzer::PeakVsTot(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_peak_vs_tot_%p", this), Form("Peak ADC vs ToT (Threshold = %d ADC);-Peak ADC [ADC];ToT", target_threshold), 200, 0, 400, 90, 1, 15);

    gStyle->SetPalette(kViridis);

    fd_.tree_->Draw(Form("tot_th%d:-peak_adc>>hist_peak_vs_tot_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "colz");

    return hist;
}