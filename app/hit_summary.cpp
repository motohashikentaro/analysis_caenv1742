// #include "./../include/rootfile_analyzer.h"
// #include "./../include/hit_analyzer.h"

// #include <TCanvas.h>
// #include <TH1D.h>
// #include <TH2D.h>

// #include <array>
// #include <string>

// int main(int argc, char* argv[]){

//     HitAnalyzer ha(argv[1]);

//     TH1D* hist_peak;
//     std::array<TH1D*, nthres> hist_charge;
//     TH1D* hist_peaktime;
//     std::array<TH2D*, nthres> hist_peak_vs_charge;
//     TH1D* hist5;

//     hist_peak = ha.PeakDistro();
//     hist_charge = ha.ChargeDistro();
//     hist_peaktime = ha.PeaktimeDistro();
//     hist_peak_vs_charge = ha.PeakVsCharge();
//     hist5 = ha.TotDistro();

//     //==================================================
//     // 1D Histograms
//     //==================================================

//     TCanvas* canvas = new TCanvas("canvas", "Hit Summary", 2400, 600);
//     canvas->Divide(4, 1);

//     canvas->cd(1);
//     hist_peak->Draw("HIST");

//     canvas->cd(2);
//     for(size_t ithres=0; ithres<nthres; ithres++){
//         if(ithres == 0){
//             hist_charge[ithres]->Draw("HIST");
//         }else{
//             hist_charge[ithres]->Draw("HIST SAME");
//         }
//     }

//     canvas->cd(3);
//     hist_peaktime->Draw("HIST");

//     canvas->cd(4);
//     hist5->Draw("HIST");

//     canvas->Update();
//     canvas->SaveAs(
//         ("./../result/" + ha.GetHitData().filename_ + "_hit_summary.png").c_str()
//     );

//     //==================================================
//     // 2D Histograms
//     //==================================================

//     TCanvas* canvas2d = new TCanvas("canvas2d", "Peak vs Charge", 1200, 1200);
//     canvas2d->Divide(2, 2);

//     for(size_t ithres=0; ithres<nthres; ithres++){
//         canvas2d->cd(ithres + 1);
//         hist_peak_vs_charge[ithres]->Draw("COLZ");
//     }

//     canvas2d->Update();
//     canvas2d->SaveAs(
//         ("./../result/" + ha.GetHitData().filename_ + "_peak_vs_charge.png").c_str()
//     );

//     return 0;
// }

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


    TH1D* hist_peak = ha.PeakDistro(ch);
    TH1D* hist_charge = ha.ChargeDistro(ch);
    TH1D* hist_peaktime = ha.PeaktimeDistro(ch);
    TH2D* hist_peak_vs_charge = ha.PeakVsCharge(ch);
    TH1D* hist_tot = ha.TotDistro(ch);


    //==================================================
    // 1D Histograms
    //==================================================

    TCanvas* canvas =
        new TCanvas("canvas", "Hit Summary", 2400, 600);

    canvas->Divide(4, 1);


    canvas->cd(1);
    hist_peak->Draw("HIST");
    ha.DrawHistInfo(hist_peak, ch, 0.15, 0.85);


    canvas->cd(2);
    hist_charge->Draw("HIST");
    ha.DrawHistInfo(hist_charge, ch, 0.55, 0.85);


    canvas->cd(3);
    hist_peaktime->Draw("HIST");
    ha.DrawHistInfo(hist_peaktime, ch, 0.55, 0.85);


    canvas->cd(4);
    hist_tot->Draw("HIST");
    ha.DrawHistInfo(hist_tot, ch, 0.55, 0.85);


    canvas->Update();

    canvas->SaveAs(
        ("./../result/" 
        + ha.GetHitData().filename_
        + "_hit_summary.png").c_str()
    );


    //==================================================
    // 2D Histogram
    //==================================================

    TCanvas* canvas2d =
        new TCanvas("canvas2d",
                    "Peak vs Charge",
                    800,
                    800);


    hist_peak_vs_charge->Draw("COLZ");


    canvas2d->Update();

    canvas2d->SaveAs(
        ("./../result/"
        + ha.GetHitData().filename_
        + "_peak_vs_charge.png").c_str()
    );


    return 0;
}