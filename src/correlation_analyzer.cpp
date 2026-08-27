#include "./../include/correlation_analyzer.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/channel_map.h"
#include "./../include/hit_reader.h"
#include "./../include/hit_extractor.h"
#include "./../include/tracker.h"

#include <vector>
#include <array>

#include <TH2D.h>

TH2D* CorrelationAnalyzer::FrontStripBackStripXCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_back_strip_x_corr_%p", this), ";Front Strip X[mm];Back Strip X[mm]", 32, -2, 2, 32, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.strip_position_front_x, rhp.strip_position_back_x);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripBackStripYCorrelation(){
    TH2D* hist = new TH2D(Form("front_strip_back_strip_y_corr_%p", this), ";Front Strip Y[mm];Back Strip Y[mm]", 32, -2, 2, 32, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.strip_position_front_y, rhp.strip_position_back_y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripHitmap(){
    TH2D* hist = new TH2D(Form("front_strip_hitmap_%p", this), ";Front Strip X[mm];Front Strip Y[mm]", 32, -2, 2, 32, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.strip_position_front_x, rhp.strip_position_front_y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripHitmap(){
    TH2D* hist = new TH2D(Form("back_strip_hitmap_%p", this), ";Back Strip X[mm];Back Strip Y[mm]", 32, -2, 2, 32, -2, 2);
    
    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.strip_position_back_x, rhp.strip_position_back_y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontDutHitmap(){
    TH2D* hist = new TH2D(Form("front_dut_hitmap_%p", this), ";Front DUT X[mm];Front DUT Y[mm]", 16, 0, 2, 16, 0, 2);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.dut_position_front.x, rhp.dut_position_front.y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackDutHitmap(){
    TH2D* hist = new TH2D(Form("back_dut_hitmap_%p", this), ";Back DUT X[mm];Back DUT Y[mm]", 16, 0, 2, 16, 0, 2);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        hist->Fill(rhp.dut_position_back.x, rhp.dut_position_back.y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontExtrapolatedTrackHitmap(){
    TH2D* hist = new TH2D(Form("front_extrapolated_track_hitmap_%p", this), ";Extrapolated DUT Position Front X [mm];Extrapolated DUT Position Front Y [mm]", 16, -2, 2, 16, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    Tracker tracker;

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        
        TrackingResult tr = tracker.Track(rhp);

        hist->Fill(tr.extrapolated_front_x, tr.extrapolated_front_y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackExtrapolatedTrackHitmap(){
    TH2D* hist = new TH2D(Form("back_extrapolated_track_hitmap_%p", this), ";Extrapolated DUT Position Back X [mm];Extrapolated DUT Position Back Y [mm]", 16, -2, 2, 16, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    Tracker tracker;

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        
        TrackingResult tr = tracker.Track(rhp);

        hist->Fill(tr.extrapolated_back_x, tr.extrapolated_back_y);
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripFrontDutXCorrelation(int cut_n_hit_ch_front){
    TH2D* hist = new TH2D(Form("front_strip_front_dut_x_corr_%p", this), ";Front Strip X[mm];Front DUT X[mm]", 32, -2, 2, 32, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        if(rhp.n_hit_ch_front >= cut_n_hit_ch_front){
            hist->Fill(rhp.strip_position_front_x, rhp.dut_position_front.x);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripBackDutXCorrelation(int cut_n_hit_ch_back){
    TH2D* hist = new TH2D(Form("back_strip_back_dut_x_corr_%p", this), ";Back Strip X[mm];Back DUT X[mm]", 32, -2, 2, 32, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        if(rhp.n_hit_ch_back >= cut_n_hit_ch_back){
            hist->Fill(rhp.strip_position_back_x, rhp.dut_position_back.x);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripBackDutXCorrelation(int cut_n_hit_ch_back){
    TH2D* hist = new TH2D(Form("front_strip_back_dut_x_corr_%p", this), ";Front Strip X[mm];Back DUT X[mm]", 32, -2, 2, 32, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        if(rhp.n_hit_ch_back >= cut_n_hit_ch_back){
            hist->Fill(rhp.strip_position_front_x, rhp.dut_position_back.x);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripFrontDutXCorrelation(int cut_n_hit_ch_front){
    TH2D* hist = new TH2D(Form("back_strip_front_dut_x_corr_%p", this), ";Back Strip X[mm];Front DUT X[mm]", 32, -2, 2, 32, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        if(rhp.n_hit_ch_front >= cut_n_hit_ch_front){
            hist->Fill(rhp.strip_position_back_x, rhp.dut_position_front.x);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripFrontDutYCorrelation(int cut_n_hit_ch_front){
    TH2D* hist = new TH2D(Form("front_strip_front_dut_y_corr_%p", this), ";Front Strip Y[mm];Front DUT Y[mm]", 32, -2, 2, 32, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        if(rhp.n_hit_ch_front >= cut_n_hit_ch_front){
            hist->Fill(rhp.strip_position_front_y, rhp.dut_position_front.y);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripBackDutYCorrelation(int cut_n_hit_ch_back){
    TH2D* hist = new TH2D(Form("back_strip_back_dut_y_corr_%p", this), ";Back Strip Y[mm];Back DUT Y[mm]", 32, -2, 2, 32, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        if(rhp.n_hit_ch_back >= cut_n_hit_ch_back){
            hist->Fill(rhp.strip_position_back_y, rhp.dut_position_back.y);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontStripBackDutYCorrelation(int cut_n_hit_ch_back){
    TH2D* hist = new TH2D(Form("front_strip_back_dut_y_corr_%p", this), ";Front Strip Y[mm];Back DUT Y[mm]", 32, -2, 2, 32, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        if(rhp.n_hit_ch_back >= cut_n_hit_ch_back){
            hist->Fill(rhp.strip_position_front_y, rhp.dut_position_back.y);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::BackStripFrontDutYCorrelation(int cut_n_hit_ch_front){
    TH2D* hist = new TH2D(Form("back_strip_front_dut_y_corr_%p", this), ";Back Strip Y[mm];Front DUT Y[mm]", 32, -2, 2, 32, 0, 4);

    MatchedHitAnalyzer mha(hd_);

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        if(rhp.n_hit_ch_front >= cut_n_hit_ch_front){
            hist->Fill(rhp.strip_position_back_y, rhp.dut_position_front.y);
        }
    });

    return hist;
}

TH2D* CorrelationAnalyzer::FrontExtrapolatedTrackDutPositionXCorrelation(int cut_n_hit_ch_front){
    TH2D* hist = new TH2D(Form("front_extrapolated_track_dut_position_x_corr_%p", this), ";Reconstructed DUT Position Front X [mm];Extrapolated DUT Position FrontX[mm]", 16, -2, 2, 16, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    Tracker tracker;

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        
        TrackingResult tr = tracker.Track(rhp);

        if(rhp.n_hit_ch_front >= cut_n_hit_ch_front){
            hist->Fill(rhp.dut_position_front.x, tr.extrapolated_front_x);
        }
    });
    return hist;
}

TH2D* CorrelationAnalyzer::FrontExtrapolatedTrackDutPositionYCorrelation(int cut_n_hit_ch_front){
    TH2D* hist = new TH2D(Form("front_extrapolated_track_dut_position_y_corr_%p", this), ";Reconstructed DUT Position Front Y [mm];Extrapolated DUT Position Front Y[mm]", 16, -2, 2, 16, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    Tracker tracker;

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        
        TrackingResult tr = tracker.Track(rhp);

        if(rhp.n_hit_ch_front >= cut_n_hit_ch_front){
            hist->Fill(rhp.dut_position_front.y, tr.extrapolated_front_y);
        }
    });
    return hist;
}

TH2D* CorrelationAnalyzer::BackExtrapolatedTrackDutPositionXCorrelation(int cut_n_hit_ch_back){
    TH2D* hist = new TH2D(Form("back_extrapolated_track_dut_position_x_corr_%p", this), ";Reconstructed DUT Position Back X [mm];Extrapolated DUT Position Back X[mm]", 16, -2, 2, 16, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    Tracker tracker;

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        
        TrackingResult tr = tracker.Track(rhp);

        if(rhp.n_hit_ch_back >= cut_n_hit_ch_back){
            hist->Fill(rhp.dut_position_back.x, tr.extrapolated_back_x);
        }
    });
    return hist;
}

TH2D* CorrelationAnalyzer::BackExtrapolatedTrackDutPositionYCorrelation(int cut_n_hit_ch_back){
    TH2D* hist = new TH2D(Form("back_extrapolated_track_dut_position_y_corr_%p", this), ";Reconstructed DUT Position Back Y [mm];Extrapolated DUT Position Back Y[mm]", 16, -2, 2, 16, -2, 2);

    MatchedHitAnalyzer mha(hd_);

    Tracker tracker;

    mha.EventLoop([&](const ReconstructedHitPosition& rhp){
        
        TrackingResult tr = tracker.Track(rhp);
        if(rhp.n_hit_ch_back >= cut_n_hit_ch_back){
            hist->Fill(rhp.dut_position_back.y, tr.extrapolated_back_y);
        }
    });
    return hist;
}