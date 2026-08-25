#ifndef CORRELATION_ANALYZER
#define CORRELATION_ANALYZER

#include "./../include/hit_reader.h"
#include "./../include/matched_hit_analyzer.h"

#include <TH2D.h>

struct CorrelationAnalyzer{
    public:
        CorrelationAnalyzer(HitData& hd): hd_(hd){};

        TH2D* FrontStripBackStripXCorrelation();
        TH2D* FrontStripBackStripYCorrelation();

        TH2D* FrontStripHitmap();
        TH2D* BackStripHitmap();
        TH2D* FrontDutHitmap();
        TH2D* BackDutHitmap();
        TH2D* FrontExtrapolatedTrackHitmap();
        TH2D* BackExtrapolatedTrackHitmap();

        TH2D* FrontStripFrontDutXCorrelation(int cut_n_hit_ch_front = 1);
        TH2D* BackStripBackDutXCorrelation(int cut_n_hit_ch_back = 1);
        TH2D* FrontStripBackDutXCorrelation(int cut_n_hit_ch_back = 1);
        TH2D* BackStripFrontDutXCorrelation(int cut_n_hit_ch_front = 1);
        TH2D* FrontStripFrontDutYCorrelation(int cut_n_hit_ch_front = 1);
        TH2D* BackStripBackDutYCorrelation(int cut_n_hit_ch_back = 1);
        TH2D* FrontStripBackDutYCorrelation(int cut_n_hit_ch_back = 1);
        TH2D* BackStripFrontDutYCorrelation(int cut_n_hit_ch_front = 1);

        TH2D* FrontExtrapolatedTrackDutPositionXCorrelation(int cut_n_hit_ch_front = 1);
        TH2D* BackExtrapolatedTrackDutPositionXCorrelation(int cut_n_hit_ch_back = 1);
        TH2D* FrontExtrapolatedTrackDutPositionYCorrelation(int cut_n_hit_ch_front = 1);
        TH2D* BackExtrapolatedTrackDutPositionYCorrelation(int cut_n_hit_ch_back = 1);

    private:
        HitData& hd_;
};

#endif