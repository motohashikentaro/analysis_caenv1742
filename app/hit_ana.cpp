#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

int main(){
    TFile* f = TFile::Open("/home/motohashi/work/analysis_caenv1742/bin/hit_feature_th30.root", "READ");
    TTree* t = (TTree*)f->Get("tree");

    TCanvas* c1 = new TCanvas("c1", "c1", 1800, 1600);
    c1->Divide(3, 2);
    c1->cd(1);
    gPad->SetLogy();
    t->Draw("tot>>h_tot(20, 0, 20)", "board==1 && ch==24 && peak_adc<-50 && tot > 2");

    c1->cd(2);
    gPad->SetLogy();
    t->Draw("peak_adc>>h_peak_adc(50, -600, 0)", "board==1 && ch==24 && peak_adc<-50 && tot > 2");

    c1->cd(3);
    gPad->SetLogy();
    t->Draw("charge>>h_charge(50, 0, 5000)", "board==1 && ch==24 && peak_adc<-50 && tot > 2");

    c1->cd(4);
    gPad->SetLogy();
    t->Draw("peak_sample>>h_peak_sample(50, 0, 1024)", "board==1 && ch==24 && peak_adc<-50 && tot > 2");

    c1->cd(5);
    t->Draw("charge:-peak_adc>>h_cp(100, 0, 2000, 100, 0, 10000)", "board==1 && ch==24 && peak_adc<-50 && tot > 2", "colz");

    c1->cd(6);
    t->Draw("charge:tot", "board==1 && ch==24 && peak_adc<-50 && tot > 2", "colz");

    c1->SaveAs("../result/hit_ana_th30.png");

    return 0;
}