#include "./../include/hit_analyzer.h"
#include "./../include/hit_reader.h"

#include <filesystem>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TColor.h>
#include <TStyle.h>

TH1D* HitAnalyzer::PeakDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_peak_%p", this), ";ADC [ADC];Entries", 100, -400, 0);

    hd_.tree_->Draw(Form("peak_adc>>hist_peak_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* HitAnalyzer::PeaktimeDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_ptime_%p", this), ";Peak Time [sample];Entries", 100, 0, 1024);

    hd_.tree_->Draw(Form("peak_time>>hist_ptime_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* HitAnalyzer::ChargeDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_charge_%p", this), ";Charge [a.u.];Entries", 100, 0, 1000);

    hd_.tree_->Draw(Form("charge>>hist_charge_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* HitAnalyzer::PeakVsCharge(int board, int ch){
    TH2D* hist = new TH2D(Form("hist_peak_vs_charge_%p", this), ";-Peak ADC [ADC];Charge [a.u.]", 200, 0, 400, 200, 0, 1000);

    hd_.tree_->Draw(Form("charge:-peak_adc>>hist_peak_vs_charge_%p", this), Form("board==%d && ch==%d", board, ch), "colz");

    return hist;
}

TH1D* HitAnalyzer::TotDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_tot_%p", this), ";ToT [sample];Entries", 100, 0, 15);

    hd_.tree_->Draw(Form("tot>>hist_tot_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* HitAnalyzer::PeakVsTot(int board, int ch){
    TH2D* hist = new TH2D(Form("hist_peak_vs_tot_%p", this), ";-Peak ADC [ADC];ToT [sample]", 200, 0, 400, 100, 0, 15);

    hd_.tree_->Draw(Form("tot:-peak_adc>>hist_peak_vs_tot_%p", this), Form("board==%d && ch==%d", board, ch), "colz");

    return hist;
}

