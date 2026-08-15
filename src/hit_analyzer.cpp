#include "./../include/hit_analyzer.h"
#include "./../include/hit_reader.h"

#include <filesystem>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TColor.h>
#include <TStyle.h>

TH1D* HitAnalyzer::PeakDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_peak_%p", this), "Peak ADC Distro;ADC [ADC];Entries", 100, -400, 0);

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));
    hist->SetStats(0);

    hd_.tree_->Draw(Form("peak_adc>>hist_peak_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* HitAnalyzer::PeaktimeDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_ptime_%p", this), "Peak Time Distro;Peak Time [sample];Entries", 100, 0, 1024);

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));
    hist->SetStats(0);

    hd_.tree_->Draw(Form("peak_time>>hist_ptime_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* HitAnalyzer::ChargeDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_charge_%p", this), "Charge Distro;Charge;Entries", 100, 1, 1000);

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));
    hist->SetStats(0);

    hd_.tree_->Draw(Form("charge>>hist_charge_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* HitAnalyzer::PeakVsCharge(int board, int ch){
    TH2D* hist = new TH2D(Form("hist_peak_vs_charge_%p", this), "Peak ADC vs Charge;-Peak ADC [ADC];Charge", 200, 0, 400, 200, 0, 1200);

    gStyle->SetPalette(kViridis);

    hd_.tree_->Draw(Form("charge:-peak_adc>>hist_peak_vs_charge_%p", this), Form("board==%d && ch==%d", board, ch), "colz");

    return hist;
}

TH1D* HitAnalyzer::TotDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_tot_%p", this), "TOT Distro;TOT [sample];Entries", 100, 0, 100);

    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));
    hist->SetStats(0);

    hd_.tree_->Draw(Form("tot>>hist_tot_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* HitAnalyzer::PeakVsTot(int board, int ch){
    TH2D* hist = new TH2D(Form("hist_peak_vs_tot_%p", this), "Peak ADC vs TOT;-Peak ADC [ADC];TOT [sample]", 200, 0, 400, 200, 0, 100);

    gStyle->SetPalette(kViridis);

    hd_.tree_->Draw(Form("tot:-peak_adc>>hist_peak_vs_tot_%p", this), Form("board==%d && ch==%d", board, ch), "colz");

    return hist;
}