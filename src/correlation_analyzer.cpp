#include "./../include/correlation_analyzer.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/channel_map.h"
#include "./../include/hit_reader.h"
#include "./../include/hit_extractor.h"
#include "./../include/tracker.h"
#include "./../include/through_event_extractor.h"
#include "./../include/through_event_reader.h"

#include <vector>
#include <array>

#include <TH2D.h>
#include <TH1D.h>
#include <TF1.h>

TH2D* CorrelationAnalyzer::FrontStripBackStripXCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_back_strip_x_corr_%p", this), ";Front Strip X[mm];Back Strip X[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("strip_position_back_x:strip_position_front_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripBackStripYCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_back_strip_y_corr_%p", this), ";Front Strip Y[mm];Back Strip Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("strip_position_back_y:strip_position_front_y>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripHitmap(){
    TH2D* hist = new TH2D(Form("front_strip_hitmap_%p", this), ";Front Strip X[mm];Front Strip Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("strip_position_front_y:strip_position_front_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripHitmap(){
    TH2D* hist = new TH2D(Form("back_strip_hitmap_%p", this), ";Back Strip X[mm];Back Strip Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("strip_position_back_y:strip_position_back_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontDutHitmap(){
    TH2D* hist = new TH2D(Form("front_dut_hitmap_%p", this), ";Front DUT X[mm];Front DUT Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_y:dut_position_front_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackDutHitmap(){
    TH2D* hist = new TH2D(Form("back_dut_hitmap_%p", this), ";Back DUT X[mm];Back DUT Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_y:dut_position_back_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontExtrapolatedTrackHitmap(){
    TH2D* hist = new TH2D(Form("front_extrapolated_track_hitmap_%p", this), ";Extrapolated DUT Position Front X [mm];Extrapolated DUT Position Front Y [mm]", 16, -2, 2, 16, -2, 2);

    td_.tree_->Draw(Form("extrapolated_front_y:extrapolated_front_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackExtrapolatedTrackHitmap(){
    TH2D* hist = new TH2D(Form("back_extrapolated_track_hitmap_%p", this), ";Extrapolated DUT Position Back X [mm];Extrapolated DUT Position Back Y [mm]", 16, -2, 2, 16, -2, 2);

    td_.tree_->Draw(Form("extrapolated_back_y:extrapolated_back_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripFrontDutXCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_front_dut_x_corr_%p", this), ";Front Strip X[mm];Front DUT X[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_x:strip_position_front_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripBackDutXCorrelation(){
    TH2D* hist = new TH2D(Form("back_strip_back_dut_x_corr_%p", this), ";Back Strip X[mm];Back DUT X[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_x:strip_position_back_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripBackDutXCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_back_dut_x_corr_%p", this), ";Front Strip X[mm];Back DUT X[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_x:strip_position_front_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripFrontDutXCorrelation(){
    TH2D* hist = new TH2D(Form("back_strip_front_dut_x_corr_%p", this), ";Back Strip X[mm];Front DUT X[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_x:strip_position_back_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripFrontDutYCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_front_dut_y_corr_%p", this), ";Front Strip Y[mm];Front DUT Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_y:strip_position_front_y>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripBackDutYCorrelation(){
    TH2D* hist = new TH2D(Form("back_strip_back_dut_y_corr_%p", this), ";Back Strip Y[mm];Back DUT Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_y:strip_position_back_y>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripBackDutYCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_back_dut_y_corr_%p", this), ";Front Strip Y[mm];Back DUT Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_y:strip_position_front_y>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripFrontDutYCorrelation(){
    TH2D* hist = new TH2D(Form("back_strip_front_dut_y_corr_%p", this), ";Back Strip Y[mm];Front DUT Y[mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_y:strip_position_back_y>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontDutPositionExtrapolatedTrackXCorrelation(){
    TH2D* hist = new TH2D(Form("front_dut_position_extrapolated_track_x_corr_%p", this), ";Extrapolated DUT Position Front X [mm];Reconstructed DUT Position Front X [mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_x:extrapolated_front_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackDutPositionExtrapolatedTrackXCorrelation(){
    TH2D* hist = new TH2D(Form("back_dut_position_extrapolated_track_x_corr_%p", this), ";Extrapolated DUT Position Back X [mm];Reconstructed DUT Position Back X [mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_x:extrapolated_back_x>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::FrontDutPositionExtrapolatedTrackYCorrelation(){
    TH2D* hist = new TH2D(Form("front_dut_position_extrapolated_track_y_corr_%p", this), ";Extrapolated DUT Position Front Y [mm];Reconstructed DUT Position Front Y [mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_y:extrapolated_front_y>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH2D* CorrelationAnalyzer::BackDutPositionExtrapolatedTrackYCorrelation(){
    TH2D* hist = new TH2D(Form("back_dut_position_extrapolated_track_y_corr_%p", this), ";Extrapolated DUT Position Back Y [mm];Reconstructed DUT Position Back Y [mm]", 32, -2, 2, 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_y:extrapolated_back_y>>%s", hist->GetName()), "", "colz");

    return hist;
}

TH1D* CorrelationAnalyzer::DifferenceFrontExtrapolatedTrackDutPositionX(){
    TH1D* hist = new TH1D(Form("difference_front_extrapolated_track_dut_position_x_%p", this), ";Reconstructed DUT Position Front X [mm] - Extrapolated DUT Position Front X [mm];Entries", 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_x - extrapolated_front_x>>%s", hist->GetName()), "", "");

    hist->Fit("gaus", "", "", -1, 0);

    return hist;
}

TH1D* CorrelationAnalyzer::DifferenceBackExtrapolatedTrackDutPositionX(){
    TH1D* hist = new TH1D(Form("difference_back_extrapolated_track_dut_position_x_%p", this), ";Reconstructed DUT Position Back X [mm] - Extrapolated DUT Position Back X [mm];Entries", 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_x - extrapolated_back_x>>%s", hist->GetName()), "", "");

    hist->Fit("gaus", "", "", -0.5, 0.7);

    return hist;
}

TH1D* CorrelationAnalyzer::DifferenceFrontExtrapolatedTrackDutPositionY(){
    TH1D* hist = new TH1D(Form("difference_front_extrapolated_track_dut_position_y_%p", this), ";Reconstructed DUT Position Front Y [mm] - Extrapolated DUT Position Front Y [mm];Entries", 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_y - extrapolated_front_y>>%s", hist->GetName()), "", "");

    hist->Fit("gaus", "", "", -1, 0.3);

    return hist;
}

TH1D* CorrelationAnalyzer::DifferenceBackExtrapolatedTrackDutPositionY(){
    TH1D* hist = new TH1D(Form("difference_back_extrapolated_track_dut_position_y_%p", this), ";Reconstructed DUT Position Back Y [mm] - Extrapolated DUT Position Back Y [mm];Entries", 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_y - extrapolated_back_y>>%s", hist->GetName()), "", "");

    hist->Fit("gaus", "", "", -1.3, -0.3);

    return hist;
}

TH1D* CorrelationAnalyzer::DifferenceFrontExtrapolatedTrackDutPositionXWithShower(){
    TH1D* hist = new TH1D(Form("difference_front_extrapolated_track_dut_position_x_with_shower_%p", this), ";Reconstructed DUT Position Front X [mm] - Extrapolated DUT Position Front X [mm];Entries", 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_x - extrapolated_front_x>>%s", hist->GetName()), "", "");

    hist->Fit("gaus", "", "", 0, 1);

    return hist;
}

TH1D* CorrelationAnalyzer::DifferenceBackExtrapolatedTrackDutPositionXWithShower(){
    TH1D* hist = new TH1D(Form("difference_back_extrapolated_track_dut_position_x_with_shower_%p", this), ";Reconstructed DUT Position Back X [mm] - Extrapolated DUT Position Back X [mm];Entries", 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_x - extrapolated_back_x>>%s", hist->GetName()), "", "");

    hist->Fit("gaus", "", "", 0.3, 1.5);

    return hist;
}

TH1D* CorrelationAnalyzer::DifferenceFrontExtrapolatedTrackDutPositionYWithShower(){
    TH1D* hist = new TH1D(Form("difference_front_extrapolated_track_dut_position_y_with_shower_%p", this), ";Reconstructed DUT Position Front Y [mm] - Extrapolated DUT Position Front Y [mm];Entries", 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_front_y - extrapolated_front_y>>%s", hist->GetName()), "", "");

    hist->Fit("gaus", "", "", -1, 0.2);

    return hist;
}

TH1D* CorrelationAnalyzer::DifferenceBackExtrapolatedTrackDutPositionYWithShower(){
    TH1D* hist = new TH1D(Form("difference_back_extrapolated_track_dut_position_y_with_shower_%p", this), ";Reconstructed DUT Position Back Y [mm] - Extrapolated DUT Position Back Y [mm];Entries", 32, -2, 2);

    td_.tree_->Draw(Form("dut_position_back_y - extrapolated_back_y>>%s", hist->GetName()), "", "");

    hist->Fit("gaus", "", "", -0.3, 1);

    return hist;
}