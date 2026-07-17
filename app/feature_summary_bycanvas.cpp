#include "./../include/rootfile_analyzer.h"
#include "./../include/feature_analyzer.h"

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>

#include <string>

int main(int argc, char* argv[])
{
    FeatureAnalyzer fa(argv[1]);

    TH1D* h_peak = fa.PeakDistro(std::stoi(argv[2]));
    TH1D* h_charge = fa.ChargeDistro(std::stoi(argv[2]));
    TH1D* h_ptime = fa.PeaktimeDistro(std::stoi(argv[2]));
    TH2D* h_peak_charge = fa.PeakVsCharge(std::stoi(argv[2]));
    TH1D* h_tot = fa.TotDistro(std::stoi(argv[2]));

    std::string output = "./../result/" + fa.GetFeatureData().filename_;


    // Peak ADC
    {
        TCanvas* c = new TCanvas("c_peak", "Peak ADC", 600, 600);

        h_peak->Draw("HIST");
        fa.DrawHistInfo(h_peak, 1, std::stoi(argv[2]), 30, 0.15, 0.85);
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);

        c->SaveAs((output + "_peak_adc.png").c_str());
        delete c;
    }


    // Charge
    {
        TCanvas* c = new TCanvas("c_charge", "Charge", 600, 600);

        h_charge->Draw("HIST");
        fa.DrawHistInfo(h_charge, 1, std::stoi(argv[2]), 30, 0.55, 0.85);
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);

        c->SaveAs((output + "_charge.png").c_str());
        delete c;
    }


    // Peak Time
    {
        TCanvas* c = new TCanvas("c_ptime", "Peak Time", 600, 600);

        h_ptime->Draw("HIST");
        fa.DrawHistInfo(h_ptime, 1, std::stoi(argv[2]), 30, 0.55, 0.85);
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);

        c->SaveAs((output + "_peak_time.png").c_str());
        delete c;
    }


    // ToT
    {
        TCanvas* c = new TCanvas("c_tot", "ToT", 600, 600);

        h_tot->Draw("HIST");
        fa.DrawHistInfo(h_tot, 1, std::stoi(argv[2]), 30, 0.55, 0.85);
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);

        c->SaveAs((output + "_tot.png").c_str());
        delete c;
    }


    // Peak ADC vs Charge
    {
        TCanvas* c = new TCanvas("c_peak_charge", "Peak vs Charge", 600, 600);

        h_peak_charge->Draw("COLZ");
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.13);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.13);

        c->SaveAs((output + "_peak_vs_charge.png").c_str());
        delete c;
    }


    return 0;
}