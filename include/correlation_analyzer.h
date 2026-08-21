#ifndef CORRELATION_ANALYZER
#define CORRELATION_ANALYZER

#include "./../include/hit_reader.h"
#include "./../include/matched_hit_analyzer.h"

#include <TH2D.h>

struct CorrelationAnalyzer{
    public:
        CorrelationAnalyzer(HitData& hd): hd_(hd){};

        TH2D* StripXCorrelation();
        TH2D* StripYCorrelation();

        TH2D* FrontStripHitmap();
        TH2D* BackStripHitmap();
        TH2D* FrontDutHitmap();
        TH2D* BackDutHitmap();

        TH2D* FrontStripFrontDutCorrelation();
        TH2D* BackStripBackDutCorrelation();
        TH2D* FrontStripBackDutCorrelation();
        TH2D* BackStripFrontDutCorrelation();

    private:
        HitData& hd_;
};

#endif