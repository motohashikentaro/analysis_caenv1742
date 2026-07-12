#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

#include <string>

int main(int argc, char* argv[]){
    TFile* file = TFile::Open(("./../data/feature/" + std::string(argv[1])).c_str(), "READ");
    TTree* tree = (TTree*)file->Get("tree");
    TCanvas* canvas = new TCanvas("c", "c", 2000, 1000);
    canvas->Divide(2, 1);

    canvas->cd(1);
    tree->Draw("peak_adc>>h_peak_adc(100, -500, 0)", "board==1 && ch==24", "");

    canvas->cd(2);
    tree->Draw("charge_th30>>h_charge_th30(100, 0, 200)", "board==1 && ch==24 && charge_th30>0", "");
    tree->Draw("charge_th50>>h_charge_th50(100, 0, 200)", "board==1 && ch==24 && charge_th50>0", "same");
    tree->Draw("charge_th70>>h_charge_th70(100, 0, 200)", "board==1 && ch==24 && charge_th70>0", "same");
    tree->Draw("charge_th90>>h_charge_th90(100, 0, 200)", "board==1 && ch==24 && charge_th90>0", "same");
    

    canvas->Update();
    canvas->SaveAs(("./../result/" + std::string(argv[1]) + "summary.png").c_str());
}