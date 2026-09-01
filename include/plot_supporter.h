#ifndef PLOT_SUPPORTER
#define PLOT_SUPPORTER

#include "./rootfile_reader.h"
#include "./feature_reader.h"
#include "./hit_reader.h"
#include "./through_event_reader.h"

#include <filesystem>
#include <optional>
#include <string>

#include <TCanvas.h>
#include <TStyle.h>
#include <TColor.h>
#include <TLatex.h>
#include <TH1.h>
#include <TH2.h>

enum class DataStage{
    Raw,
    Feature,
    Hit,
    ThroughEvent
};

struct PlotContext{
    DataStage stage;

    std::string run_number;

    std::optional<std::string> feature_condition;
    std::optional<std::string> tracker_condition;
    std::optional<std::string> dut_condition;
    std::optional<std::string> strip_reconstruction_condition;
    std::optional<std::string> pixel_reconstruction_condition;
};

class PlotSupporter{
    public:
        PlotSupporter(const RootData& rd);
        PlotSupporter(const FeatureData& fd);
        PlotSupporter(const HitData& hd);
        PlotSupporter(const ThroughEventData& td);

        static TCanvas* MakeCanvas1D(const char* name, int nx, int ny);
        static TCanvas* MakeCanvas2D(const char* name, int nx, int ny);

        static void SetPadStyle1D(TPad* pad);
        static void SetPadStyle2D(TPad* pad);

        static void SetHistStyle1D(TH1* hist);
        static void SetHistStyle2D(TH2* hist);

        std::filesystem::path MakeSaveDir() const;

    private:
        PlotContext context_;
};

#endif