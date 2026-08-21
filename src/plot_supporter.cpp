#include "./../include/plot_supporter.h"

#include <TCanvas.h>
#include <TStyle.h>
#include <TColor.h>
#include <TLatex.h>
#include <TH1.h>
#include <TH2.h>



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