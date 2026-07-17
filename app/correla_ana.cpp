// #include "./../include/hit_correlation.h"

// #include <iostream>

// #include <TFile.h>
// #include <TTree.h>
// #include <TH1D.h>
// #include <TCanvas.h>
// #include <TStyle.h>
// #include <TLegend.h>

// int main(){
//     TFile* file = TFile::Open("./../data/correlation/correlation.root", "READ");
//     TTree* tree = (TTree*)file->Get("tree");

//     EventHit eh;

//     int energy;
//     int nw;

//     tree->SetBranchAddress(
//         "event",
//         &eh.event
//     );


//     tree->SetBranchAddress(
//         "nfront",
//         &eh.nfront
//     );


//     tree->SetBranchAddress(
//         "front_charge",
//         &eh.front_charge
//     );


//     tree->SetBranchAddress(
//         "front_max_adc",
//         &eh.front_max_adc
//     );


//     tree->SetBranchAddress(
//         "nback",
//         &eh.nback
//     );


//     tree->SetBranchAddress(
//         "back_charge",
//         &eh.back_charge
//     );


//     tree->SetBranchAddress(
//         "back_max_adc",
//         &eh.back_max_adc
//     );

//     tree->SetBranchAddress(
//         "energy",
//         &energy
//     );

//     tree->SetBranchAddress(
//         "nw",
//         &nw
//     );

//     constexpr int nenergy = 5;

//     std::array<TH1D*, nenergy> hist_fcharge_sh;

//     for(int i=0; i<nenergy; i++){

//         hist_fcharge_sh[i] = new TH1D(
//             Form("h_fcharge_sh_%d_2", i+1),
//             "Front Charge in Shower;Charge;Events",
//             100,
//             0,
//             10000
//         );
//     }

//     Long64_t nentries = tree->GetEntries();

//     for(Long64_t i=0; i<nentries; i++){

//         tree->GetEntry(i);

//         if(nw != 2)
//             continue;

//         if(eh.nfront <= 0 || eh.nback <= 0)
//             continue;

//         if(energy < 1 || energy > 5)
//             continue;

//         hist_fcharge_sh[energy-1]->Fill(
//             eh.front_charge
//         );
//     }

//     TCanvas* c1 = new TCanvas(
//         "c1",
//         "Front Charge",
//         800,
//         600
//     );

//     int colors[] = {
//         kBlack,
//         kRed,
//         kBlue,
//         kGreen+2,
//         kMagenta
//     };

//     TLegend* leg = new TLegend(
//         0.65,
//         0.65,
//         0.88,
//         0.88
//     );

//     for(int i=0; i<nenergy; i++){

//         hist_fcharge_sh[i]->SetLineColor(colors[i]);
//         hist_fcharge_sh[i]->SetLineWidth(2);

//         if(i==0)
//             hist_fcharge_sh[i]->Draw("hist");
//         else
//             hist_fcharge_sh[i]->Draw("hist same");

//         leg->AddEntry(
//             hist_fcharge_sh[i],
//             Form("%d GeV", i+1),
//             "l"
//         );
//     }

//     leg->Draw();

//     c1->SaveAs("./../result/front_charge_vs_energy_W2.png");



//     file->Close();

//     return 0;
// }

#include "./../include/correla_ana.h"

int main()
{

    CorrelaAna ana(
        "./../data/correlation/correlation.root"
    );


    ana.Analyze();

    ana.Draw();

    ana.Save(
        "./../result"
    );


    return 0;
}