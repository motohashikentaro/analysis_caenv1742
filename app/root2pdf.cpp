#include <iostream>
#include <string>

#include <TFile.h>
#include <TKey.h>
#include <TCanvas.h>
#include <TH2.h>
#include <TStyle.h>

int main(int argc, char* argv[]){

    if(argc != 3){
        std::cerr
            << "Usage: " << argv[0]
            << " <input.root> <output.pdf>"
            << std::endl;
        return 1;
    }

    const std::string input_file = argv[1];
    const std::string output_pdf = argv[2];

    TFile file(input_file.c_str(), "READ");

    if(file.IsZombie()){
        std::cerr << "Failed to open: " << input_file << std::endl;
        return 1;
    }

    TCanvas canvas("canvas", "canvas", 800, 800);

    gStyle->SetOptStat(0);
    gStyle->SetPaintTextFormat(".2f");

    // PDFを開く
    canvas.Print((output_pdf + "[").c_str());

    TIter next(file.GetListOfKeys());
    TKey* key = nullptr;

    int page = 0;

    while((key = static_cast<TKey*>(next()))){

        TObject* obj = key->ReadObj();

        TH2* hist = dynamic_cast<TH2*>(obj);

        if(!hist){
            delete obj;
            continue;
        }

        canvas.Clear();

        hist->SetStats(false);
        hist->SetMarkerSize(1.5);

        hist->Draw("COLZ TEXT");

        canvas.Print(output_pdf.c_str());

        ++page;

        delete obj;
    }

    // PDFを閉じる
    canvas.Print((output_pdf + "]").c_str());

    std::cout << "Saved: " << output_pdf << std::endl;
    std::cout << "Number of pages: " << page << std::endl;

    return 0;
}