#include "./../include/rootfile_analyzer.h"
#include "./../include/hit_analyzer.h"

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>

#include <string>


int main(int argc, char* argv[])
{
    HitAnalyzer ha(argv[1]);

    int ch = std::stoi(argv[2]);


    TH1D* h_peak = ha.PeakDistro(ch);
    TH1D* h_charge = ha.ChargeDistro(ch);
    TH1D* h_ptime = ha.PeaktimeDistro(ch);
    TH2D* h_peak_charge = ha.PeakVsCharge(ch);
    TH1D* h_tot = ha.TotDistro(ch);


    std::string output =
        "./../result/" + ha.GetHitData().filename_;


    //==================================================
    // Peak ADC
    //==================================================
    {
        TCanvas* c = new TCanvas("c_peak", "Peak ADC", 600, 600);

        h_peak->Draw("HIST");

        ha.DrawHistInfo(h_peak, ch, 0.15, 0.85);

        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);

        c->SaveAs((output + "_peak_adc.png").c_str());

        delete c;
    }


    //==================================================
    // Charge
    //==================================================
    {
        TCanvas* c = new TCanvas("c_charge", "Charge", 600, 600);

        h_charge->Draw("HIST");

        ha.DrawHistInfo(h_charge, ch, 0.55, 0.85);

        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);

        c->SaveAs((output + "_charge.png").c_str());

        delete c;
    }


    //==================================================
    // Peak Time
    //==================================================
    {
        TCanvas* c = new TCanvas("c_ptime", "Peak Time", 600, 600);

        h_ptime->Draw("HIST");

        ha.DrawHistInfo(h_ptime, ch, 0.55, 0.85);

        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);

        c->SaveAs((output + "_peak_time.png").c_str());

        delete c;
    }


    //==================================================
    // ToT
    //==================================================
    {
        TCanvas* c = new TCanvas("c_tot", "ToT", 600, 600);

        h_tot->Draw("HIST");

        ha.DrawHistInfo(h_tot, ch, 0.55, 0.85);

        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);

        c->SaveAs((output + "_tot.png").c_str());

        delete c;
    }


    //==================================================
    // Peak ADC vs Charge
    //==================================================
    {
        TCanvas* c = new TCanvas("c_peak_charge",
                                  "Peak vs Charge",
                                  600,
                                  600);

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