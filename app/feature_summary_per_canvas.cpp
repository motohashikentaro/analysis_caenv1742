#include "./../include/feature_analyzer.h"
#include "./../include/feature_reader.h"

#include <string>

#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TStyle.h>

int main(int argc, char* argv[]){
    FeatureReader fr(argv[1]);
    FeatureAnalyzer fa(fr.GetFeatureData());

    int board = std::stoi(argv[2]);
    int ch = std::stoi(argv[3]);
    int target_threshold = std::stoi(argv[4]);

    TH1D* h_peak = fa.PeakDistro(board, ch);
    TH1D* h_ptime = fa.PeaktimeDistro(board, ch);
    TH1D* h_charge = fa.ChargeDistro(board, ch, target_threshold);
    TH2D* h_peak_charge = fa.PeakVsCharge(board, ch, target_threshold);
    TH1D* h_tot = fa.TotDistro(board, ch, target_threshold);
    TH2D* h_peak_tot = fa.PeakVsTot(board, ch, target_threshold);

    std::string output = "./../result/run_" + fr.GetFeatureData().run_number_ + "_board_" + std::to_string(board) + "_ch_" + std::to_string(ch) + "_threshold_" + std::to_string(target_threshold) + "_feature";

    TLatex* latex = new TLatex();
    latex->SetNDC();
    latex->SetTextSize(0.04);

    gStyle->SetOptStat(0);

    // Peak ADC
    {
        TCanvas* c = new TCanvas("c_peak", "Peak ADC", 600, 600);

        h_peak->Draw("HIST");
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);
        latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

        c->SaveAs((output + "_peak_adc.png").c_str());
        delete c;
    }

    // Peak Time
    {
        TCanvas* c = new TCanvas("c_ptime", "Peak Time", 600, 600);

        h_ptime->Draw("HIST");
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);
        latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

        c->SaveAs((output + "_peak_time.png").c_str());
        delete c;
    }

    // Charge
    {
        TCanvas* c = new TCanvas("c_charge", "Charge", 600, 600);

        h_charge->Draw("HIST");
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);
        latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

        c->SaveAs((output + "_charge.png").c_str());
        delete c;
    }

    // ToT
    {
        TCanvas* c = new TCanvas("c_tot", "ToT", 600, 600);

        h_tot->Draw("HIST");
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.08);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.08);
        latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

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
        latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

        c->SaveAs((output + "_peak_vs_charge.png").c_str());
        delete c;
    }

    // Peak ADC vs ToT
    {
        TCanvas* c = new TCanvas("c_peak_tot", "Peak vs ToT", 600, 600);

        h_peak_tot->Draw("COLZ");
        gPad->SetLeftMargin(0.13);
        gPad->SetRightMargin(0.13);
        gPad->SetBottomMargin(0.13);
        gPad->SetTopMargin(0.13);
        latex->DrawLatex(0.70, 0.85, Form("Run %s", (fr.GetFeatureData()).run_number_.c_str()));
        latex->DrawLatex(0.70, 0.80, Form("Board %d, Ch %d", board, ch));
        latex->DrawLatex(0.70, 0.75, Form("Threshold %d ADC", target_threshold));

        c->SaveAs((output + "_peak_vs_tot.png").c_str());
        delete c;
    }

    return 0;
}