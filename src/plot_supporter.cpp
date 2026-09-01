#include "./../include/plot_supporter.h"

#include <filesystem>
#include <stdexcept>

#include <TCanvas.h>
#include <TStyle.h>
#include <TColor.h>
#include <TLatex.h>
#include <TH1.h>
#include <TH2.h>

PlotSupporter::PlotSupporter(const RootData& rd): context_{
    DataStage::Raw,
    rd.run_number_,
    std::nullopt,
    std::nullopt,
    std::nullopt,
    std::nullopt,
    std::nullopt
}{}

PlotSupporter::PlotSupporter(const FeatureData& fd): context_{
    DataStage::Feature,
    fd.run_number_,
    fd.feature_condition_,
    std::nullopt,
    std::nullopt,
    std::nullopt,
    std::nullopt
}{}

PlotSupporter::PlotSupporter(const HitData& hd): context_{
    DataStage::Hit,
    hd.run_number_,
    hd.feature_condition_,
    hd.tracker_condition_,
    hd.dut_condition_,
    std::nullopt,
    std::nullopt
}{}

PlotSupporter::PlotSupporter(const ThroughEventData& td): context_{
    DataStage::ThroughEvent,
    td.run_number_,
    td.feature_condition_,
    td.tracker_condition_,
    td.dut_condition_,
    td.strip_reconstruction_condition_,
    td.pixel_reconstruction_condition_
}{}

TCanvas* PlotSupporter::MakeCanvas1D(const char* name, int nx, int ny){
    constexpr int pad_width = 600;
    constexpr int pad_height = 600;

    auto* canvas = new TCanvas(name, name, pad_width*nx, pad_height*ny);
    canvas->Divide(nx, ny, 0.0, 0.0);

    for(int i=0; i<nx*ny; ++i){
        canvas->cd(i+1);
        SetPadStyle1D(static_cast<TPad*>(gPad));
    }
    return canvas;
}

TCanvas* PlotSupporter::MakeCanvas2D(const char* name, int nx, int ny){
    constexpr int pad_height = 600;

    constexpr double left = 0.12;
    constexpr double right = 0.15;
    constexpr double top = 0.03;
    constexpr double bottom = 0.12;

    constexpr int pad_width = static_cast<int>(pad_height * (1.0 - top - bottom) / (1.0 - left - right));

    auto* canvas = new TCanvas(name, name, pad_width*nx, pad_height*ny);
    canvas->Divide(nx, ny, 0.0, 0.0);

    for(int i=0; i<nx*ny; ++i){
        canvas->cd(i+1);
        SetPadStyle2D(static_cast<TPad*>(gPad));
    }
    return canvas;
}

void PlotSupporter::SetPadStyle1D(TPad* pad){
    pad->SetLeftMargin(0.12);
    pad->SetRightMargin(0.03);
    pad->SetTopMargin(0.03);
    pad->SetBottomMargin(0.12);
}

void PlotSupporter::SetPadStyle2D(TPad* pad){
    pad->SetLeftMargin(0.12);
    pad->SetRightMargin(0.15);
    pad->SetTopMargin(0.03);
    pad->SetBottomMargin(0.12);
}

void PlotSupporter::SetHistStyle1D(TH1* hist){
    hist->SetLineWidth(2);
    hist->SetLineColor(0);
    hist->SetFillColor(TColor::GetColor("#008899"));

    hist->GetXaxis()->SetTitleSize(0.045);
    hist->GetYaxis()->SetTitleSize(0.045);

    hist->GetXaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetLabelSize(0.04);

    hist->GetXaxis()->SetTitleOffset(1.0);
    hist->GetYaxis()->SetTitleOffset(1.1);    
}

void PlotSupporter::SetHistStyle2D(TH2* hist){
    gStyle->SetPalette(kViridis);
    
    hist->GetXaxis()->SetTitleSize(0.045);
    hist->GetYaxis()->SetTitleSize(0.045);

    hist->GetXaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetLabelSize(0.04);

    hist->GetXaxis()->SetTitleOffset(1.0);
    hist->GetYaxis()->SetTitleOffset(1.1);    
}

std::filesystem::path PlotSupporter::MakeSaveDir() const{
    std::filesystem::path save_dir = std::filesystem::path("./../result") / ("run_" + context_.run_number);

    switch(context_.stage){
        case DataStage::Raw:
            save_dir /= "analysis";
            break;

        case DataStage::Feature:
            if(!context_.feature_condition){
                throw std::runtime_error("Feature condition is not set for feature data.");
            }
            save_dir /= std::filesystem::path("feature") / *context_.feature_condition;
            break;

        case DataStage::Hit:
            if(!context_.feature_condition || !context_.tracker_condition || !context_.dut_condition){
                throw std::runtime_error("Feature, tracker, or DUT condition is not set for hit data.");
            }
            save_dir /= std::filesystem::path("hit") / *context_.feature_condition / (*context_.tracker_condition + "-" + *context_.dut_condition);
            break;
        case DataStage::ThroughEvent:
            if(!context_.feature_condition || !context_.tracker_condition || !context_.dut_condition || !context_.strip_reconstruction_condition || !context_.pixel_reconstruction_condition){
                throw std::runtime_error("Feature, tracker, DUT, strip reconstruction, or pixel reconstruction condition is not set for through event data.");
            }
            save_dir /= std::filesystem::path("through_event") / *context_.feature_condition / (*context_.tracker_condition + "-" + *context_.dut_condition) / (*context_.strip_reconstruction_condition + "-" + *context_.pixel_reconstruction_condition);
            break;
    }
    std::filesystem::create_directories(save_dir);
    return save_dir;
}