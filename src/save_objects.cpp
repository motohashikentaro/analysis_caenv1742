#include <iostream>
#include <filesystem>

#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

#include "./../include/rootfile_analyzer.h"

void SaveObjects_per_board_ch(const std::vector<TObject*>& objects,
                              const std::string& output_name
                              ){
    const int nobj = objects.size();
    int ncols = std::ceil(std::sqrt(nobj));
    int nrows = std::ceil(ncols / static_cast<double>(nobj));

    int pad_size = 600;
    int width = ncols * pad_size;
    int height = nrows * pad_size;

    TCanvas* c = new TCanvas("c", "c", width, height);
    c->Divide(ncols, nrows);

    for(size_t i=0; i<objects.size(); ++i){
        c->cd(i+1);
        TObject* obj = objects[i];

        if(auto gr = dynamic_cast<TGraph*>(obj)){
            gPad->SetRightMargin(0.17);
            gPad->SetTopoMargin(0.17);
            gr->Draw("ALP");
        }

        gPad->Update();
    }

    c->SaveAs(("./../result/" + output_name).c_str());

    delete c;

}