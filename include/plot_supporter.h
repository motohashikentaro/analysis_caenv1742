#ifndef PLOT_SUPPORTER
#define PLOT_SUPPORTER

#include <TCanvas.h>
#include <TStyle.h>
#include <TColor.h>
#include <TLatex.h>
#include <TH1.h>
#include <TH2.h>

class PlotSupporter{
    public:
        static TCanvas* MakeCanvas1D(const char* name, int nx, int ny);
        static TCanvas* MakeCanvas2D(const char* name, int nx, int ny);

        static void SetPadStyle1D(TPad* pad);
        static void SetPadStyle2D(TPad* pad);

        static void SetHistStyle1D(TH1* hist);
        static void SetHistStyle2D(TH2* hist);
};

#endif