#include "./../include/feature_analyzer.h"
#include "./../include/feature_reader.h"

#include <filesystem>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TColor.h>
#include <TStyle.h>

TH1D* FeatureAnalyzer::PeakDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_peak_%p", this), ";ADC [ADC];Entries", 100, -400, 0);

    fd_.tree_->Draw(Form("peak_adc>>hist_peak_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* FeatureAnalyzer::PeaktimeDistro(int board, int ch){
    TH1D* hist = new TH1D(Form("hist_ptime_%p", this), ";Peak Time [sample];Entries", 100, 0, 1024);

    fd_.tree_->Draw(Form("peak_time>>hist_ptime_%p", this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* FeatureAnalyzer::ChargeDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_charge_%p", this), ";Charge [a.u.];Entries", 100, 0, 1000);

    fd_.tree_->Draw(Form("charge_th%d>>hist_charge_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* FeatureAnalyzer::PeakVsCharge(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_peak_vs_charge_%p", this), ";Peak ADC [ADC];Charge [a.u.]", 200, 0, 400, 200, 0, 1000);

    fd_.tree_->Draw(Form("charge_th%d:-peak_adc>>hist_peak_vs_charge_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "colz");

    return hist;
}

TH1D* FeatureAnalyzer::TotDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_tot_%p", this), ";ToT [sample];Entries", 100, 0, 15);

    fd_.tree_->Draw(Form("tot_th%d>>hist_tot_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* FeatureAnalyzer::PeakVsTot(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_peak_vs_tot_%p", this), ";Peak ADC [ADC];ToT [sample]", 200, 0, 400, 100, 0, 15);

    fd_.tree_->Draw(Form("tot_th%d:-peak_adc>>hist_peak_vs_tot_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "colz");

    return hist;
}

TH1D* FeatureAnalyzer::ChargePerPeakDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_charge_per_peak_%p", this), ";Charge per Peak;Entries", 100, 0, 10);

    fd_.tree_->Draw(Form("charge_th%d/(-peak_adc)>>hist_charge_per_peak_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* FeatureAnalyzer::TotPerPeakDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_tot_per_peak_%p", this), ";ToT per Peak;Entries", 100, 0, 0.5);

    fd_.tree_->Draw(Form("tot_th%d/(-peak_adc)>>hist_tot_per_peak_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* FeatureAnalyzer::ChargeMinusAlphaAdcDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_charge_minus_alpha_adc_%p", this), ";Charge - alpha * Peak ADC;Entries", 100, -400, 400);

    fd_.tree_->Draw(Form("charge_th%d - 1 * peak_adc>>hist_charge_minus_alpha_adc_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}