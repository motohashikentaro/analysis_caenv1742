#include "./../include/correlation_analyzer.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/channel_map.h"
#include "./../include/hit_reader.h"
#include "./../include/hit_extractor.h"

#include <vector>
#include <array>

#include <TH2D.h>

TH2D* CorrelationAnalyzer::StripXCorrelation(){
    TH2D* hist = new TH2D(Form("strip_x_corr_%p", this), ";Front Strip X;Back Strip X", 16, 0, 8, 16, 0, 8);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.strip_position_front_x, rhp.strip_position_back_x);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::StripYCorrelation(){
    TH2D* hist = new TH2D(Form("strip_y_corr_%p", this), ";Front Strip Y;Back Strip Y", 16, 0, 8, 16, 0, 8);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.strip_position_front_y, rhp.strip_position_back_y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripHitmap(){
    TH2D* hist = new TH2D(Form("front_strip_hitmap_%p", this), ";Front Strip X;Front Strip Y", 16, 0, 8, 16, 0, 8);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.strip_position_front_x, rhp.strip_position_front_y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripHitmap(){
    TH2D* hist = new TH2D(Form("back_strip_hitmap_%p", this), ";Back Strip X;Back Strip Y", 16, 0, 8, 16, 0, 8);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.strip_position_back_x, rhp.strip_position_back_y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontDutHitmap(){
    TH2D* hist = new TH2D(Form("front_dut_hitmap_%p", this), ";Front DUT X;Front DUT Y", 4, 0, 4, 4, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        for(const auto& pixel_pos : rhp.pixel_positions[0]){
            hist->Fill(pixel_pos.x, pixel_pos.y);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackDutHitmap(){
    TH2D* hist = new TH2D(Form("back_dut_hitmap_%p", this), ";Back DUT X;Back DUT Y", 4, 0, 4, 4, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        for(const auto& pixel_pos : rhp.pixel_positions[1]){
            hist->Fill(pixel_pos.x, pixel_pos.y);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripFrontDutCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_front_dut_corr_%p", this), ";Front Strip X;Front DUT X", 16, 0, 8, 4, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        for(const auto& pixel_pos : rhp.pixel_positions[0]){
            hist->Fill(rhp.strip_position_front_x, pixel_pos.x);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripBackDutCorrelation(){
    TH2D* hist = new TH2D(Form("back_strip_back_dut_corr_%p", this), ";Back Strip X;Back DUT X", 16, 0, 8, 4, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        for(const auto& pixel_pos : rhp.pixel_positions[1]){
            hist->Fill(rhp.strip_position_back_x, pixel_pos.x);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripBackDutCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_back_dut_corr_%p", this), ";Front Strip X;Back DUT X", 16, 0, 8, 4, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        for(const auto& pixel_pos : rhp.pixel_positions[1]){
            hist->Fill(rhp.strip_position_front_x, pixel_pos.x);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripFrontDutCorrelation(){
    TH2D* hist = new TH2D(Form("back_strip_front_dut_corr_%p", this), ";Back Strip X;Front DUT X", 16, 0, 8, 4, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        for(const auto& pixel_pos : rhp.pixel_positions[0]){
            hist->Fill(rhp.strip_position_back_x, pixel_pos.x);
        }
    });

    return hist;
}