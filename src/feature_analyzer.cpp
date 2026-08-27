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

    fd_.tree_->Draw(Form("charge_th%d:-peak_adc>>hist_peak_vs_charge_%p", target_threshold, this), Form("board==%d && ch==%d && charge_th%d > 0", board, ch, target_threshold), "colz");

    return hist;
}

TH1D* FeatureAnalyzer::TotDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_tot_%p", this), ";ToT [sample];Entries", 100, 0, 15);

    fd_.tree_->Draw(Form("tot_th%d>>hist_tot_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* FeatureAnalyzer::PeakVsTot(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_peak_vs_tot_%p", this), ";Peak ADC [ADC];ToT [sample]", 200, 0, 400, 100, 0, 15);

    fd_.tree_->Draw(Form("tot_th%d:-peak_adc>>hist_peak_vs_tot_%p", target_threshold, this), Form("board==%d && ch==%d && tot_th%d > 0", board, ch, target_threshold), "colz");

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
    TH1D* hist = new TH1D(Form("hist_charge_minus_alpha_adc_%p", this), ";Charge - alpha * Peak ADC;Entries", 100, -200, 200);

    fd_.tree_->Draw(Form("charge_th%d + 2.5 * peak_adc>>hist_charge_minus_alpha_adc_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* FeatureAnalyzer::TotMinusAlphaAdcDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_tot_minus_alpha_adc_%p", this), ";ToT - alpha * Peak ADC;Entries", 100, -10, 10);

    fd_.tree_->Draw(Form("tot_th%d + 0.04 * peak_adc>>hist_tot_minus_alpha_adc_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH1D* FeatureAnalyzer::SlopeDistro(int board, int ch, int target_threshold){
    TH1D* hist = new TH1D(Form("hist_slope_%p", this), ";Raise Slope [ADC/sample];Entries", 100, 0, 80);

    fd_.tree_->Draw(Form("-raise_slope_th%d>>hist_slope_%p", target_threshold, this), Form("board==%d && ch==%d", board, ch), "");

    return hist;
}

TH2D* FeatureAnalyzer::TotVsSlope(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_tot_vs_slope_%p", this), ";ToT [sample];Raise Slope [ADC/sample]", 100, 0, 15, 100, 0, 80);

    fd_.tree_->Draw(Form("-raise_slope_th%d:tot_th%d>>hist_tot_vs_slope_%p", target_threshold, target_threshold, this), Form("board==%d && ch==%d && -peak_adc > %d", board, ch, target_threshold), "colz");

    return hist;
}

TH2D* FeatureAnalyzer::PeakVsSlope(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_peak_vs_slope_%p", this), ";Peak ADC [ADC];Raise Slope [ADC/sample]", 200, 0, 400, 100, 0, 80);

    fd_.tree_->Draw(Form("-raise_slope_th%d:-peak_adc>>hist_peak_vs_slope_%p", target_threshold, this), Form("board==%d && ch==%d && -peak_adc > %d", board, ch, target_threshold), "colz");

    return hist;
}

TH2D* FeatureAnalyzer::ChargeVsSlope(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_charge_vs_slope_%p", this), ";Charge [ADC];Raise Slope [ADC/sample]", 200, 0, 400, 100, 0, 80);

    fd_.tree_->Draw(Form("-raise_slope_th%d:charge_th%d>>hist_charge_vs_slope_%p", target_threshold, target_threshold, this), Form("board==%d && ch==%d && -peak_adc > %d", board, ch, target_threshold), "colz");

    return hist;
}

TH2D* FeatureAnalyzer::PeaktimeVsSlope(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_peaktime_vs_slope_%p", this), ";Peak Time [sample];Raise Slope [ADC/sample]", 100, 0, 1024, 100, 0, 80);

    fd_.tree_->Draw(Form("-raise_slope_th%d:peak_time>>hist_peaktime_vs_slope_%p", target_threshold, this), Form("board==%d && ch==%d && -peak_adc > %d", board, ch, target_threshold), "colz");

    return hist;
}

TH2D* FeatureAnalyzer::PeakVsPeaktime(int board, int ch, int target_threshold){
    TH2D* hist = new TH2D(Form("hist_peak_vs_peaktime_%p", this), ";Peak Time [sample];Peak ADC [ADC]", 100, 0, 1024, 200, 0, 400);

    fd_.tree_->Draw(Form("-peak_adc:peak_time>>hist_peak_vs_peaktime_%p", this), Form("board==%d && ch==%d && -peak_adc > %d", board, ch, target_threshold), "colz");

    return hist;
}

std::vector<Long64_t> FeatureAnalyzer::SelectEvents(int board, int ch, size_t max_events, const std::function<bool(const EvtFeature&)>& condition) {
    std::vector<Long64_t> selected_events;
    selected_events.reserve(max_events);  // Reserve space to avoid multiple allocations

    for(Long64_t entry=0; entry<fd_.tree_->GetEntries(); ++entry){
        fd_.tree_->GetEntry(entry);
        const EvtFeature& ef = fd_.ef_;

        if(ef.board != board) continue;
        if(ef.ch != ch) continue;

        if(!condition(ef)) continue;

        selected_events.push_back(ef.evt);

        if(selected_events.size() >= max_events) break;  // Stop if we reached the maximum number of events
    }

    return selected_events;
}