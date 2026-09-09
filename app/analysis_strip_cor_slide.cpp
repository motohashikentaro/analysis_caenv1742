#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"

#include <filesystem>

#include <TH1D.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TLatex.h>
#include <TStyle.h>

int main(){
    ThroughEventReader tr2forx("/home/motohashi/work/analysis_caenv1742/data/through_event/correct_threshold/CorrectThresTracker-RejectAll/Standard-ShowerFullHit/through_event_05434-05438.root");
    ThroughEventData& td2forx = tr2forx.GetThroughEventData();

    ThroughEventReader tr5fory("/home/motohashi/work/analysis_caenv1742/data/through_event/correct_threshold/CorrectThresTracker-RejectAll/Standard-ShowerFullHit/through_event_55555.root");
    ThroughEventData& td5fory = tr5fory.GetThroughEventData();

    TH1D* x_correlation_hist = new TH1D("x_correlation_hist", "X Correlation Histogram", 32, -2, 2);
    TH1D* y_correlation_hist = new TH1D("y_correlation_hist", "Y Correlation Histogram", 32, -2, 2);

    TH2D* x_y_correlation_hist = new TH2D("x_y_correlation_hist", ";Front Strip X[mm];Back Strip X[mm]", 32, -2, 2, 32, -2, 2);

    td2forx.tree_->Draw("strip_position_front_x - strip_position_back_x>>x_correlation_hist", "strip_position_front_x >= -1 && strip_position_front_x <= 1", "");
    td5fory.tree_->Draw("strip_position_front_y - strip_position_back_y>>y_correlation_hist", "", "");

    td2forx.tree_->Draw(
        "strip_position_back_x:strip_position_front_x>>x_y_correlation_hist",
        "strip_position_front_x >= -1 && strip_position_front_x <= 1",
        "colz"
    );
    x_correlation_hist->SetTitle(
        ";x_{front} - x_{back} [mm];Entries"
    );

    y_correlation_hist->SetTitle(
        ";y_{front} - y_{back} [mm];Entries"
    );    
    auto pramary_color = TColor::GetColor("#008899");

    x_correlation_hist->SetFillColor(pramary_color);
    y_correlation_hist->SetFillColor(pramary_color);

    x_correlation_hist->SetLineWidth(0);
    y_correlation_hist->SetLineWidth(0);
    gStyle->SetPalette(kViridis);

    TF1* fit_x = new TF1("fit_x", "gaus", -1.2, 0.3);
    x_correlation_hist->Fit(fit_x, "R");
    double mean_x  = fit_x->GetParameter(1);
    double sigma_x = fit_x->GetParameter(2);

    TF1* fit_y = new TF1("fit_y", "gaus", -1.2, 0.3);
    y_correlation_hist->Fit(fit_y, "R");
    double mean_y  = fit_y->GetParameter(1);
    double sigma_y = fit_y->GetParameter(2);

    gStyle->SetOptStat(0);

    PlotSupporter ps2(td2forx);
    TCanvas* canvas_x = PlotSupporter::MakeCanvas1D("x_correlation_canvas", 1, 1);
    canvas_x->cd();
    x_correlation_hist->Draw();
    fit_x->Draw("same");
    TLatex latex_x;
    latex_x.SetNDC();
    latex_x.SetTextSize(0.03);
    latex_x.DrawLatexNDC(0.6, 0.8, Form("#mu = %.3f", mean_x));
    latex_x.DrawLatexNDC(0.6, 0.7, Form("#sigma = %.3f", sigma_x));
    canvas_x->SaveAs((ps2.MakeSaveDir() / "x_correlation_hist.png").string().c_str());

    TCanvas* canvas_y = PlotSupporter::MakeCanvas1D("y_correlation_canvas", 1, 1);
    canvas_y->cd();
    y_correlation_hist->Draw();
    fit_y->Draw("same");
    TLatex latex_y;
    latex_y.SetNDC();
    latex_y.SetTextSize(0.03);
    latex_y.DrawLatexNDC(0.6, 0.8, Form("#mu = %.3f", mean_y));
    latex_y.DrawLatexNDC(0.6, 0.7, Form("#sigma = %.3f", sigma_y));
    canvas_y->SaveAs((ps2.MakeSaveDir() / "y_correlation_hist.png").string().c_str());

    TCanvas* canvas_xy = PlotSupporter::MakeCanvas2D("x_y_correlation_canvas", 1, 1);
    canvas_xy->cd();
    x_y_correlation_hist->Draw("colz");
    canvas_xy->SaveAs((ps2.MakeSaveDir() / "x_y_correlation_hist.png").string().c_str());
}
