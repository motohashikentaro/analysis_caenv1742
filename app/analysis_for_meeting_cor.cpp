#include "./../include/through_event_reader.h"
#include "./../include/plot_supporter.h"
#include "./../include/alignment_data.h"

#include <filesystem>
#include <array>
#include <iostream>

#include <TH1D.h>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TH2D.h>

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cerr << "Usage: " << argv[0] << " <through_event_root_file>" << std::endl;
        return 1;
    }
    ThroughEventReader tr(argv[1]);
    ThroughEventData& td = tr.GetThroughEventData();

    NewAlignmentDutData dad;
    NewAlignmentStripData sad;

    gStyle->SetOptStat(0);

    const double xmin_frontx = -2.0 - dad.front_x;
    const double xmax_frontx =  2.0 - dad.front_x;
    const double ymin_fronty = -2.0 - dad.front_y;
    const double ymax_fronty =  2.0 - dad.front_y;
    const double xmin_backx  = -2.0 - dad.back_x;
    const double xmax_backx  =  2.0 - dad.back_x;
    const double ymin_backy  = -2.0 - dad.back_y;
    const double ymax_backy  =  2.0 - dad.back_y;

    const double exmin_x = -2.0 - sad.x;
    const double exmax_x =  2.0 - sad.x;
    const double exmin_y = -2.0 - sad.y;
    const double exmax_y =  2.0 - sad.y;

    TH2D* reco_ex_correlation_front_x = new TH2D(
        "reco_ex_correlation_front_x",
        ";Extrapolated Front X[mm];Reconstructed Front X[mm]",
        32, exmin_x, exmax_x,
        32, xmin_frontx, xmax_frontx
    );

    TH2D* reco_ex_correlation_front_y = new TH2D(
        "reco_ex_correlation_front_y",
        ";Extrapolated Front Y[mm];Reconstructed Front Y[mm]",
        32, exmin_y, exmax_y,
        32, ymin_fronty, ymax_fronty
    );

    TH2D* reco_ex_correlation_back_x = new TH2D(
        "reco_ex_correlation_back_x",
        ";Extrapolated Back X[mm];Reconstructed Back X[mm]",
        32, exmin_x, exmax_x,
        32, xmin_backx, xmax_backx
    );
    
    TH2D* reco_ex_correlation_back_y = new TH2D(
        "reco_ex_correlation_back_y",
        ";Extrapolated Back Y[mm];Reconstructed Back Y[mm]",
        32, exmin_y, exmax_y,
        32, ymin_backy, ymax_backy
    );

    td.tree_->Draw(Form("dut_position_front_x:extrapolated_front_x>>%s", reco_ex_correlation_front_x->GetName()), "", "colz");
    td.tree_->Draw(Form("dut_position_front_y:extrapolated_front_y>>%s", reco_ex_correlation_front_y->GetName()), "", "colz");
    td.tree_->Draw(Form("dut_position_back_x:extrapolated_back_x>>%s", reco_ex_correlation_back_x->GetName()), "", "colz");
    td.tree_->Draw(Form("dut_position_back_y:extrapolated_back_y>>%s", reco_ex_correlation_back_y->GetName()), "", "colz");

    PlotSupporter ps(td);

    std::filesystem::path save_dir = ps.MakeSaveDir();

    auto canvas = PlotSupporter::MakeCanvas2D("reco_ex_correlation_canvas", 2, 2);
    std::array<TH2D*, 4> hist_array = {
        reco_ex_correlation_front_x,
        reco_ex_correlation_front_y,
        reco_ex_correlation_back_x,
        reco_ex_correlation_back_y
    };
    for(int i = 0; i < hist_array.size(); ++i){
        canvas->cd(i + 1);
        PlotSupporter::SetHistStyle2D(hist_array[i]);
        hist_array[i]->Draw("colz");
    }
    canvas->SaveAs((save_dir / "slide_reco_ex_correlation.png").string().c_str());
}