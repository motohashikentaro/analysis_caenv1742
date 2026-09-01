#ifndef CORRELATION_ANALYZER
#define CORRELATION_ANALYZER

#include "./../include/hit_reader.h"
#include "./../include/matched_hit_analyzer.h"
#include "./../include/through_event_reader.h"

#include <TH2D.h>

struct CorrelationAnalyzer{
    public:
        CorrelationAnalyzer(ThroughEventData& td): td_(td){};

        TH2D* FrontStripBackStripXCorrelation();
        TH2D* FrontStripBackStripYCorrelation();

        TH2D* FrontStripHitmap();
        TH2D* BackStripHitmap();
        TH2D* FrontDutHitmap();
        TH2D* BackDutHitmap();
        TH2D* FrontExtrapolatedTrackHitmap();
        TH2D* BackExtrapolatedTrackHitmap();

        TH2D* FrontStripFrontDutXCorrelation();
        TH2D* BackStripBackDutXCorrelation();
        TH2D* FrontStripBackDutXCorrelation();
        TH2D* BackStripFrontDutXCorrelation();
        TH2D* FrontStripFrontDutYCorrelation();
        TH2D* BackStripBackDutYCorrelation();
        TH2D* FrontStripBackDutYCorrelation();
        TH2D* BackStripFrontDutYCorrelation();

        TH2D* FrontDutPositionExtrapolatedTrackXCorrelation();
        TH2D* BackDutPositionExtrapolatedTrackXCorrelation();
        TH2D* FrontDutPositionExtrapolatedTrackYCorrelation();
        TH2D* BackDutPositionExtrapolatedTrackYCorrelation();

        TH1D* DifferenceFrontExtrapolatedTrackDutPositionX();
        TH1D* DifferenceBackExtrapolatedTrackDutPositionX();
        TH1D* DifferenceFrontExtrapolatedTrackDutPositionY();
        TH1D* DifferenceBackExtrapolatedTrackDutPositionY();

        TH1D* DifferenceFrontExtrapolatedTrackDutPositionXWithShower();
        TH1D* DifferenceBackExtrapolatedTrackDutPositionXWithShower();
        TH1D* DifferenceFrontExtrapolatedTrackDutPositionYWithShower();
        TH1D* DifferenceBackExtrapolatedTrackDutPositionYWithShower();

    private:
        ThroughEventData& td_;
};

#endif