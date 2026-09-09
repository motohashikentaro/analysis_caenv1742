#include "./../include/plot_supporter.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <TH1D.h>
#include <TCanvas.h>

int main(){

    std::ifstream ifs("/home/motohashi/work/analysis_caenv1742/data/etc/HITS_alignmentruns.txt");

    if(!ifs.is_open()){
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    TH1D* hist_x = new TH1D("hist_x", ";Residual; Entries", 50, -4, 4);
    TH1D* hist_y = new TH1D("hist_y", ";Residual; Entries", 50, -4, 4);

    std::string line;

    while(std::getline(ifs, line)){
        std::stringstream ss(line);

        std::string type;
        std::string layer_str;
        std::string axis;
        std::string value_str;

        std::getline(ss, type, ',');
        std::getline(ss, layer_str, ',');
        std::getline(ss, axis, ',');
        std::getline(ss, value_str, ',');

        const double value = std::stod(value_str);

        if(axis == "X"){
            hist_x->Fill(value);
        } else if(axis == "Y"){
            hist_y->Fill(value);
        }
    }

    auto canvas = PlotSupporter::MakeCanvas1D("canvas", 2, 1);
    PlotSupporter::SetHistStyle1D(hist_x);
    PlotSupporter::SetHistStyle1D(hist_y);
    canvas->cd(1);
    hist_x->Draw();
    canvas->cd(2);
    hist_y->Draw();

    canvas->SaveAs("/home/motohashi/work/analysis_caenv1742/result/etc/HITS_alignment.png");

    return 0;
}