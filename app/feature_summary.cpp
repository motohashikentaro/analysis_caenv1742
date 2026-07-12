#include "./../include/rootfile_analyzer.h"
#include "./../include/feature_analyzer.h"

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

#include <string>

int main(int argc, char* argv[]){
    FeatureAnalyzer fa(argv[1]);
    TH1D* hist1;
    std::array<TH1D*, nthres> hist2;
    TH1D* hist3;
    std::array<TH2D*, nthres> hist4;

    hist1 = fa.PeakDistro();
    hist2 = fa.ChargeDistro();
    hist3 = fa.PeaktimeDistro();
    hist4 = fa.PeakVsCharge();

    TCanvas* canvas = new TCanvas("canvas", "canvas", 1800, 600);
    TCanvas* canvas_2d = new TCanvas("canvas2d", "canvas2d", 1200, 1200);
    canvas->Divide(3, 1);
    canvas->cd(1);
    hist1->Draw("HIST");

    canvas->cd(2);
    for(size_t ithres=0; ithres<nthres; ithres++){
        if(ithres==0){
            hist2[ithres]->Draw("HIST");
        }else{
            hist2[ithres]->Draw("HIST SAME");
        }
    }

    canvas->cd(3);
    hist3->Draw("HIST");

    canvas->Update();
    canvas->SaveAs(("./../result/" + (fa.GetFeatureData()).filename_ + "_feature_summary.png").c_str());

    canvas_2d->Divide(2, 2);
    for(size_t ithres=0; ithres<nthres; ithres++){
        canvas_2d->cd(ithres+1);
        hist4[ithres]->Draw("colz");
    }

    canvas_2d->Update();
    canvas_2d->SaveAs(("./../result/" + (fa.GetFeatureData()).filename_ + "_paek_vs_charge.png").c_str());

    return 0;
}